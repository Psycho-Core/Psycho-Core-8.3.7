/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Engine - the per-state decision loop. One Engine instance drives one BotState
 *  (NonCombat / Combat / Dead). Each tick DoNextAction():
 *      1. ProcessTriggers   - poll active triggers, queue their handler actions
 *      2. PushDefaultActions - queue each active strategy's default actions
 *      3. pop highest-relevance basket, apply multipliers, resolve the action
 *      4. if useful+possible -> Execute (and queue its continuers), else try
 *         its prerequisites / alternatives; repeat until something fires.
 *
 *  Strategies are added/removed by name; their TriggerNodes, Multipliers and
 *  default-action packs are (re)collected from the AiObjectContext lazily.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_ENGINE_H
#define PSYCHOBOT_ENGINE_ENGINE_H

#include "Strategy.h"
#include "Queue.h"
#include "Multiplier.h"
#include "Trigger.h"
#include "Event.h"
#include <list>
#include <map>
#include <string>

namespace psychobot
{
    class PsychobotAI;
    class AiObjectContext;
    class ActionNode;
    class Action;

    class Engine
    {
    public:
        Engine(PsychobotAI* ai, AiObjectContext* context, BotState state);
        ~Engine();

        // Run one decision tick. Returns the name of the executed action (or "").
        std::string DoNextAction(Event const& event = Event(), int depth = 0);

        // Strategy management (by name, resolved through the context).
        bool AddStrategy(std::string const& name);
        bool RemoveStrategy(std::string const& name);
        bool HasStrategy(std::string const& name) const;
        void ClearStrategies();
        std::string ListStrategies() const;

    private:
        void  Reinit();                       // (re)collect triggers/multipliers/defaults
        void  ProcessTriggers();
        void  PushDefaultActions();
        bool  MultiplyAndPush(NextAction** actions, float forceRelevance,
                              bool skipPrerequisites, Event const& event);
        float ApplyMultipliers(Action* action, float relevance);

        // Resolve an ActionNode by name (strategy node first, else bare node).
        ActionNode* ResolveActionNode(std::string const& name);

        PsychobotAI*     _ai;
        AiObjectContext* _context;            // not owned (owned by PsychobotAI)
        BotState         _state;

        // Active strategies (owned by the context cache; we just track names).
        std::list<Strategy*> _strategies;

        // Collected each Reinit() from the active strategies.
        std::list<TriggerNode*> _triggers;    // owned here
        std::list<Multiplier*>  _multipliers; // not owned (context cache)

        // ActionNodes the engine created lazily (owned here, freed on dtor).
        std::map<std::string, ActionNode*> _ownedNodes;

        Queue _queue;
        bool  _initialized = false;

        static int const kMaxDepth = 5;       // recursion guard for chains
    };
}

#endif // PSYCHOBOT_ENGINE_ENGINE_H
