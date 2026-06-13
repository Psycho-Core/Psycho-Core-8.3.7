/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Warrior STRATEGIES (S8) - second class vertical slice (Arms/Fury/Protection).
 *    GenericWarriorStrategy - shared: Charge gap-closer, defensive CD when low.
 *    ArmsWarriorStrategy    - Mortal Strike, Overpower (procs MS), Execute
 *                             (<20% / Sudden Death), Slam (rage dump),
 *                             Whirlwind AoE, Colossus Smash CD.
 *    FuryWarriorStrategy    - Rampage (Enrage at 80 rage), Bloodthirst (heal/
 *                             Enrage), Raging Blow, Execute, Whirlwind AoE,
 *                             Recklessness CD.
 *    ProtectionWarriorStrategy - Shield Slam, Revenge, Thunder Clap AoE,
 *                             Ignore Pain / Shield Block mitigation, Devastate
 *                             filler, Shield Wall defensive CD.
 *    GenericWarriorNonCombatStrategy - stance upkeep (Battle/Defensive).
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_WARRIOR_STRATEGIES_H
#define PSYCHOBOT_WARRIOR_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericWarriorStrategy : public Strategy
    {
    public:
        GenericWarriorStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "warrior"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ArmsWarriorStrategy : public GenericWarriorStrategy
    {
    public:
        ArmsWarriorStrategy(PsychobotAI* ai) : GenericWarriorStrategy(ai) { }
        std::string GetName() const override { return "arms"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class FuryWarriorStrategy : public GenericWarriorStrategy
    {
    public:
        FuryWarriorStrategy(PsychobotAI* ai) : GenericWarriorStrategy(ai) { }
        std::string GetName() const override { return "fury"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ProtectionWarriorStrategy : public GenericWarriorStrategy
    {
    public:
        ProtectionWarriorStrategy(PsychobotAI* ai) : GenericWarriorStrategy(ai) { }
        std::string GetName() const override { return "protection"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericWarriorNonCombatStrategy : public Strategy
    {
    public:
        GenericWarriorNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "warrior nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterWarriorStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_WARRIOR_STRATEGIES_H
