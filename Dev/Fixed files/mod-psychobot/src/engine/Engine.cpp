/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "Engine.h"
#include "AiObjectContext.h"
#include "ActionNode.h"
#include "Action.h"

namespace psychobot
{
    Engine::Engine(PsychobotAI* ai, AiObjectContext* context, BotState state)
        : _ai(ai), _context(context), _state(state) { }

    Engine::~Engine()
    {
        for (TriggerNode* node : _triggers)
            delete node;
        _triggers.clear();

        for (auto& pair : _ownedNodes)
            delete pair.second;
        _ownedNodes.clear();
    }

    // ----------------------------------------------------------------------
    // Strategy management
    // ----------------------------------------------------------------------
    bool Engine::AddStrategy(std::string const& name)
    {
        if (HasStrategy(name))
            return false;

        Strategy* strategy = _context ? _context->GetStrategy(name) : nullptr;
        if (!strategy)
            return false;

        _strategies.push_back(strategy);
        _initialized = false;     // force a Reinit() on next tick
        return true;
    }

    bool Engine::RemoveStrategy(std::string const& name)
    {
        for (auto it = _strategies.begin(); it != _strategies.end(); ++it)
        {
            if ((*it)->GetName() == name)
            {
                _strategies.erase(it);
                _initialized = false;
                return true;
            }
        }
        return false;
    }

    bool Engine::HasStrategy(std::string const& name) const
    {
        for (Strategy* s : _strategies)
            if (s->GetName() == name)
                return true;
        return false;
    }

    void Engine::ClearStrategies()
    {
        _strategies.clear();
        _initialized = false;
    }

    std::string Engine::ListStrategies() const
    {
        std::string out;
        for (Strategy* s : _strategies)
        {
            if (!out.empty())
                out += ", ";
            out += s->GetName();
        }
        return out;
    }

    // ----------------------------------------------------------------------
    // Reinit - collect triggers/multipliers from the active strategies.
    // ----------------------------------------------------------------------
    void Engine::Reinit()
    {
        for (TriggerNode* node : _triggers)
            delete node;
        _triggers.clear();
        _multipliers.clear();

        for (Strategy* strategy : _strategies)
        {
            strategy->InitTriggers(_triggers, _state);
            strategy->InitMultipliers(_multipliers, _state);
        }

        _initialized = true;
    }

    // ----------------------------------------------------------------------
    // ResolveActionNode - prefer a strategy-declared node (so its P/A/C chains
    // apply); otherwise build & cache a bare node wrapping the same-named action.
    // ----------------------------------------------------------------------
    ActionNode* Engine::ResolveActionNode(std::string const& name)
    {
        for (Strategy* strategy : _strategies)
            if (ActionNode* node = strategy->GetAction(name))
                return node;

        auto it = _ownedNodes.find(name);
        if (it != _ownedNodes.end())
            return it->second;

        ActionNode* node = new ActionNode(name);
        _ownedNodes[name] = node;
        return node;
    }

    // ----------------------------------------------------------------------
    // ProcessTriggers - poll active triggers; queue their handler actions.
    // ----------------------------------------------------------------------
    void Engine::ProcessTriggers()
    {
        for (TriggerNode* node : _triggers)
        {
            if (!node->GetTrigger())
                node->SetTrigger(_context ? _context->GetTrigger(node->GetName()) : nullptr);

            Trigger* trigger = node->GetTrigger();
            if (!trigger || !trigger->IsActive())
                continue;

            Event event = trigger->GetEvent();
            MultiplyAndPush(node->GetHandlers(), 0.0f, false, event);
        }
    }

    void Engine::PushDefaultActions()
    {
        for (Strategy* strategy : _strategies)
            MultiplyAndPush(strategy->GetDefaultActions(_state), 0.0f, false, Event());
    }

    // ----------------------------------------------------------------------
    // MultiplyAndPush - queue each NextAction in `actions` as an ActionBasket.
    // forceRelevance overrides the NextAction's own relevance when > 0.
    // ----------------------------------------------------------------------
    bool Engine::MultiplyAndPush(NextAction** actions, float forceRelevance,
                                 bool skipPrerequisites, Event const& event)
    {
        if (!actions)
            return false;

        bool pushed = false;
        for (int i = 0; actions[i]; ++i)
        {
            NextAction* na = actions[i];
            float relevance = (forceRelevance > 0.0f) ? forceRelevance : na->GetRelevance();

            ActionNode* node = ResolveActionNode(na->GetName());
            if (!node)
                continue;

            _queue.Push(new ActionBasket(node, relevance, skipPrerequisites, event));
            pushed = true;
        }

        // The handler array was a fresh allocation (Clone/Merge/Array); free it.
        NextAction::Destroy(actions);
        return pushed;
    }

    float Engine::ApplyMultipliers(Action* action, float relevance)
    {
        if (!action)
            return relevance;
        for (Multiplier* m : _multipliers)
            if (m)
                relevance *= m->GetValue(action);
        return relevance;
    }

    // ----------------------------------------------------------------------
    // DoNextAction - one decision tick.
    // ----------------------------------------------------------------------
    std::string Engine::DoNextAction(Event const& event, int depth)
    {
        if (depth > kMaxDepth)
            return std::string();

        if (depth == 0)
        {
            if (!_initialized)
                Reinit();

            _queue.RemoveExpired(2 /*seconds*/);
            ProcessTriggers();
            PushDefaultActions();
        }

        // Pop highest-relevance baskets until one executes or the queue drains.
        ActionBasket* basket = nullptr;
        while (ActionNode* node = _queue.Pop(&basket))
        {
            float relevance = basket ? basket->GetRelevance() : 0.0f;
            Event basketEvent = basket ? basket->GetEvent() : event;
            bool skipPrereq = basket ? basket->IsSkipPrerequisites() : false;
            delete basket;
            basket = nullptr;

            // Bind the action into the node (lazily resolved from the context).
            Action* action = nullptr;
            if (node->GetAction())
            {
                action = node->GetAction();
            }
            else if (_context)
            {
                action = _context->GetAction(node->GetName());
                node->SetAction(action);
            }

            if (!action)
                continue;

            float finalRelevance = ApplyMultipliers(action, relevance);
            if (finalRelevance <= 0.0f)
                continue;

            // Prerequisites must be satisfied (queued ahead) unless skipped.
            if (!skipPrereq)
            {
                if (NextAction** prereqs = node->GetPrerequisites())
                {
                    if (NextAction::Size(prereqs) > 0)
                    {
                        // Push prereqs slightly above this action, requeue self.
                        MultiplyAndPush(prereqs, finalRelevance + 0.02f, false, basketEvent);
                        _queue.Push(new ActionBasket(node, finalRelevance + 0.01f, true, basketEvent));
                        continue;
                    }
                    NextAction::Destroy(prereqs);
                }
            }

            if (action->IsUseful() && action->IsPossible())
            {
                if (action->Execute(basketEvent))
                {
                    // Queue continuers to run next.
                    MultiplyAndPush(node->GetContinuers(), 0.0f, false, basketEvent);
                    return action->GetName();
                }
                // Failed to execute -> try alternatives.
                MultiplyAndPush(node->GetAlternatives(), finalRelevance + 0.03f, false, basketEvent);
            }
            else
            {
                // Not runnable now -> try alternatives.
                MultiplyAndPush(node->GetAlternatives(), finalRelevance + 0.03f, false, basketEvent);
            }
        }

        // If new baskets were queued by alternatives/continuers, recurse once.
        if (!_queue.Empty())
            return DoNextAction(event, depth + 1);

        return std::string();
    }
}
