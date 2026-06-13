/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Demon Hunter STRATEGIES (S18 - NEW class, authored from BfA 8.3 research).
 *  The final class - only TWO specs.
 *    GenericDemonHunterStrategy - shared: Consume Magic interrupt, Fel Rush
 *                                 gap-closer, Metamorphosis CD.
 *    HavocDemonHunterStrategy (dps) - Demon's Bite (Fury gen), Chaos Strike
 *                                 (Fury spend), Blade Dance, Eye Beam CD,
 *                                 Immolation Aura, Throw Glaive ranged filler.
 *    VengeanceDemonHunterStrategy (tank) - Fracture/Shear (Pain + fragments),
 *                                 Soul Cleave (Pain spend + heal), Spirit Bomb
 *                                 (fragment AoE), Demon Spikes + Fiery Brand
 *                                 mitigation, Immolation Aura, Sigil of Flame.
 *    GenericDemonHunterNonCombatStrategy - (no major upkeep; placeholder hook).
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DEMONHUNTER_STRATEGIES_H
#define PSYCHOBOT_DEMONHUNTER_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericDemonHunterStrategy : public Strategy
    {
    public:
        GenericDemonHunterStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "demon hunter"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class HavocDemonHunterStrategy : public GenericDemonHunterStrategy
    {
    public:
        HavocDemonHunterStrategy(PsychobotAI* ai) : GenericDemonHunterStrategy(ai) { }
        std::string GetName() const override { return "havoc"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class VengeanceDemonHunterStrategy : public GenericDemonHunterStrategy
    {
    public:
        VengeanceDemonHunterStrategy(PsychobotAI* ai) : GenericDemonHunterStrategy(ai) { }
        std::string GetName() const override { return "vengeance"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericDemonHunterNonCombatStrategy : public Strategy
    {
    public:
        GenericDemonHunterNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "demon hunter nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterDemonHunterStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_DEMONHUNTER_STRATEGIES_H
