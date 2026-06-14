/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Monk STRATEGIES (S17 - NEW class, authored from BfA 8.3 research).
 *    GenericMonkStrategy - shared: Expel Harm self-heal, Spear Hand Strike
 *                          interrupt, Touch of Death execute.
 *    BrewmasterMonkStrategy (tank) - Keg Smash (Shuffle), Blackout Strike,
 *                          Breath of Fire, Tiger Palm filler, Ironskin Brew +
 *                          Purifying Brew mitigation, Spinning Crane Kick AoE.
 *    MistweaverMonkStrategy (healer) - Renewing Mist HoT, Vivify, Enveloping
 *                          Mist, Essence Font (AoE); Tiger Palm damage filler.
 *    WindwalkerMonkStrategy (dps) - Tiger Palm (Chi gen), Rising Sun Kick +
 *                          Blackout Kick (Chi spend), Fists of Fury CD, Spinning
 *                          Crane Kick AoE.
 *    GenericMonkNonCombatStrategy - Legacy of the Emperor / stat buff upkeep.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_MONK_STRATEGIES_H
#define PSYCHOBOT_MONK_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericMonkStrategy : public Strategy
    {
    public:
        GenericMonkStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "monk"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class BrewmasterMonkStrategy : public GenericMonkStrategy
    {
    public:
        BrewmasterMonkStrategy(PsychobotAI* ai) : GenericMonkStrategy(ai) { }
        std::string GetName() const override { return "brewmaster"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class MistweaverMonkStrategy : public GenericMonkStrategy
    {
    public:
        MistweaverMonkStrategy(PsychobotAI* ai) : GenericMonkStrategy(ai) { }
        std::string GetName() const override { return "mistweaver"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_HEAL | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class WindwalkerMonkStrategy : public GenericMonkStrategy
    {
    public:
        WindwalkerMonkStrategy(PsychobotAI* ai) : GenericMonkStrategy(ai) { }
        std::string GetName() const override { return "windwalker"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericMonkNonCombatStrategy : public Strategy
    {
    public:
        GenericMonkNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "monk nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterMonkStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_MONK_STRATEGIES_H
