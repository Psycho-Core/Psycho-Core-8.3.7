/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "Strategy.h"

namespace psychobot
{
    Strategy::Strategy(PsychobotAI* ai)
        : AiNamedObject(ai, "strategy") { }

    Strategy::~Strategy() = default;

    void Strategy::InitTriggers(std::list<TriggerNode*>& triggers, BotState state)
    {
        switch (state)
        {
            case BotState::Combat:    InitCombatTriggers(triggers);    break;
            case BotState::NonCombat: InitNonCombatTriggers(triggers); break;
            case BotState::Dead:      InitDeadTriggers(triggers);      break;
        }
    }

    void Strategy::InitMultipliers(std::list<Multiplier*>& multipliers, BotState state)
    {
        switch (state)
        {
            case BotState::Combat:    InitCombatMultipliers(multipliers);    break;
            case BotState::NonCombat: InitNonCombatMultipliers(multipliers); break;
            case BotState::Dead:      InitDeadMultipliers(multipliers);      break;
        }
    }

    NextAction** Strategy::GetDefaultActions(BotState state)
    {
        switch (state)
        {
            case BotState::Combat:    return GetDefaultCombatActions();
            case BotState::NonCombat: return GetDefaultNonCombatActions();
            case BotState::Dead:      return GetDefaultDeadActions();
        }
        return nullptr;
    }

    ActionNode* Strategy::GetAction(std::string const& name)
    {
        return _actionNodeFactory.GetObject(name, GetAI());
    }
}
