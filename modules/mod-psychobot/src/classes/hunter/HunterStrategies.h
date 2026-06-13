/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Hunter STRATEGIES (S10) - Beast Mastery / Marksmanship / Survival + pets.
 *    GenericHunterStrategy - shared: Kill Shot execute, Exhilaration self-heal.
 *    BeastMasteryHunterStrategy - ranged pet spec: Barbed Shot (Frenzy/Focus),
 *      Kill Command (needs pet), Cobra Shot (Focus dump), Bestial Wrath CD,
 *      Multi-Shot (Beast Cleave AoE). Pet REQUIRED.
 *    MarksmanshipHunterStrategy - ranged: Aimed Shot (key), Arcane Shot (Precise
 *      Shots dump), Rapid Fire CD, Steady Shot (Focus builder), Serpent Sting
 *      DoT, Multi-Shot AoE, Trueshot CD.
 *    SurvivalHunterStrategy - MELEE pet spec: Kill Command (Focus), Wildfire
 *      Bomb (key DoT), Raptor Strike (Focus dump, melee), Serpent Sting DoT,
 *      Carve AoE, Coordinated Assault CD. Pet REQUIRED.
 *    GenericHunterNonCombatStrategy - pet upkeep (Call Pet / Revive / Mend) +
 *      Aspect of the Cheetah-style buffs left to later; Aspect of the Hawk-like
 *      passive in 8.3 so no toggle needed.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_HUNTER_STRATEGIES_H
#define PSYCHOBOT_HUNTER_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericHunterStrategy : public Strategy
    {
    public:
        GenericHunterStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "hunter"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class BeastMasteryHunterStrategy : public GenericHunterStrategy
    {
    public:
        BeastMasteryHunterStrategy(PsychobotAI* ai) : GenericHunterStrategy(ai) { }
        std::string GetName() const override { return "beast mastery"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class MarksmanshipHunterStrategy : public GenericHunterStrategy
    {
    public:
        MarksmanshipHunterStrategy(PsychobotAI* ai) : GenericHunterStrategy(ai) { }
        std::string GetName() const override { return "marksmanship"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class SurvivalHunterStrategy : public GenericHunterStrategy
    {
    public:
        SurvivalHunterStrategy(PsychobotAI* ai) : GenericHunterStrategy(ai) { }
        std::string GetName() const override { return "survival"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericHunterNonCombatStrategy : public Strategy
    {
    public:
        GenericHunterNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "hunter nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterHunterStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_HUNTER_STRATEGIES_H
