/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Strategy - a named bundle of behaviour: it supplies (a) the trigger->action
 *  bindings to install, (b) any relevance multipliers, (c) a set of default
 *  actions to push when nothing else fires, and (d) its own ActionNode factory
 *  (so a strategy can declare per-action P/A/C chains). The engine asks every
 *  active strategy to contribute these when its strategy set changes.
 *
 *  A class spec strategy (S7+) is just a Strategy that fills InitCombatTriggers
 *  + GetDefaultCombatActions with its 8.3 rotation - the reference pattern.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_STRATEGY_H
#define PSYCHOBOT_ENGINE_STRATEGY_H

#include "AiObject.h"
#include "Trigger.h"
#include "Multiplier.h"
#include "ActionNode.h"
#include "NamedObjectContext.h"
#include "NextAction.h"
#include <list>
#include <string>

namespace psychobot
{
    // Which engine a strategy contributes to (the bot keeps one engine per
    // state and swaps the active one as its situation changes).
    enum class BotState : uint8
    {
        NonCombat = 0,
        Combat    = 1,
        Dead      = 2
    };

    // Coarse classification (lets the AI pick melee vs ranged default packs).
    enum StrategyType
    {
        STRATEGY_TYPE_GENERIC  = 0,
        STRATEGY_TYPE_COMBAT   = 1,
        STRATEGY_TYPE_TANK     = 2,
        STRATEGY_TYPE_DPS      = 4,
        STRATEGY_TYPE_HEAL     = 8,
        STRATEGY_TYPE_RANGED   = 16,
        STRATEGY_TYPE_MELEE    = 32,
        STRATEGY_TYPE_NONCOMBAT = 64
    };

    class Strategy : public AiNamedObject
    {
    public:
        explicit Strategy(PsychobotAI* ai);
        virtual ~Strategy();

        // Install this strategy's bindings/multipliers for a given state.
        // Caller (Engine) owns the resulting TriggerNode*/Multiplier* lists.
        void InitTriggers(std::list<TriggerNode*>& triggers, BotState state);
        void InitMultipliers(std::list<Multiplier*>& multipliers, BotState state);

        virtual NextAction** GetDefaultActions(BotState state);
        virtual int          GetType() { return STRATEGY_TYPE_GENERIC; }
        virtual std::string  GetName() const override = 0;

        // Resolve a named ActionNode from this strategy's factory (nullptr if
        // the strategy doesn't declare that node - the engine then falls back
        // to a bare ActionNode wrapping the action of the same name).
        virtual ActionNode* GetAction(std::string const& name);

    protected:
        // Default-action packs (one per state).
        virtual NextAction** GetDefaultCombatActions()    { return nullptr; }
        virtual NextAction** GetDefaultNonCombatActions() { return nullptr; }
        virtual NextAction** GetDefaultDeadActions()      { return nullptr; }

        // Trigger wiring (one per state). Push TriggerNode* into the list.
        virtual void InitCombatTriggers(std::list<TriggerNode*>& /*t*/)    { }
        virtual void InitNonCombatTriggers(std::list<TriggerNode*>& /*t*/) { }
        virtual void InitDeadTriggers(std::list<TriggerNode*>& /*t*/)      { }

        // Multiplier wiring (one per state). Push Multiplier* into the list.
        virtual void InitCombatMultipliers(std::list<Multiplier*>& /*m*/)    { }
        virtual void InitNonCombatMultipliers(std::list<Multiplier*>& /*m*/) { }
        virtual void InitDeadMultipliers(std::list<Multiplier*>& /*m*/)      { }

        // Per-strategy ActionNode factory (subclasses register node creators).
        NamedObjectContext<ActionNode> _actionNodeFactory;
    };
}

#endif // PSYCHOBOT_ENGINE_STRATEGY_H
