/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Druid STRATEGIES (S16) - Balance/Feral dps, Guardian tank, Restoration heal
 *  (FOUR specs) + FORMS. Each combat spec first shifts into its form, then runs
 *  its rotation:
 *    GenericDruidStrategy - shared: Regrowth self-heal, Barkskin defensive.
 *    BalanceDruidStrategy (Moonkin Form) - Moonfire/Sunfire DoTs, Wrath filler,
 *      Starsurge (AP spender), Starfall AoE.
 *    FeralDruidStrategy (Cat Form) - Rake/Rip bleeds, Shred builder, Ferocious
 *      Bite finisher, Tiger's Fury CD, Swipe AoE; combo points.
 *    GuardianDruidStrategy (Bear Form, tank) - Mangle, Thrash bleed, Ironfur +
 *      Frenzied Regeneration mitigation, Maul rage dump, Moonfire.
 *    RestorationDruidStrategy (caster) - Rejuvenation/Regrowth/Wild Growth/
 *      Swiftmend on the party member to heal; Moonfire/Wrath damage filler.
 *    GenericDruidNonCombatStrategy - Mark of the Wild buff upkeep.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DRUID_STRATEGIES_H
#define PSYCHOBOT_DRUID_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericDruidStrategy : public Strategy
    {
    public:
        GenericDruidStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "druid"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class BalanceDruidStrategy : public GenericDruidStrategy
    {
    public:
        BalanceDruidStrategy(PsychobotAI* ai) : GenericDruidStrategy(ai) { }
        std::string GetName() const override { return "balance"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class FeralDruidStrategy : public GenericDruidStrategy
    {
    public:
        FeralDruidStrategy(PsychobotAI* ai) : GenericDruidStrategy(ai) { }
        std::string GetName() const override { return "feral"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GuardianDruidStrategy : public GenericDruidStrategy
    {
    public:
        GuardianDruidStrategy(PsychobotAI* ai) : GenericDruidStrategy(ai) { }
        std::string GetName() const override { return "guardian"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationDruidStrategy : public GenericDruidStrategy
    {
    public:
        RestorationDruidStrategy(PsychobotAI* ai) : GenericDruidStrategy(ai) { }
        std::string GetName() const override { return "restoration"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_HEAL | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericDruidNonCombatStrategy : public Strategy
    {
    public:
        GenericDruidNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "druid nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterDruidStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_DRUID_STRATEGIES_H
