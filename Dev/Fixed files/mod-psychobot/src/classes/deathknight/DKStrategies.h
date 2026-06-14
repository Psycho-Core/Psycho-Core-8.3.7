/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Death Knight STRATEGIES (S7) - the first full class vertical slice.
 *  Mirrors the ike3/cmangos per-class pattern adapted to our engine:
 *    GenericDKStrategy    - shared DK combat (diseases upkeep, Death Strike heal,
 *                           interrupt window, AoE Death and Decay).
 *    BloodDKStrategy      - tank: Marrowrend (Bone Shield), Heart Strike,
 *                           Death Strike (heal/RP), Blood Boil (Blood Plague AoE),
 *                           Death and Decay, Death Grip pull, Dancing Rune Weapon.
 *    FrostDKStrategy      - dps: Howling Blast (Rime/Frost Fever), Obliterate
 *                           (Killing Machine), Frost Strike (RP), Remorseless
 *                           Winter, Pillar of Frost CD, Frostscythe AoE.
 *    UnholyDKStrategy     - dps: Outbreak (Virulent Plague), Festering Strike,
 *                           Scourge Strike, Death Coil (RP/Sudden Doom),
 *                           Death and Decay AoE, Apocalypse/Army CD, Raise Dead.
 *    GenericDKNonCombatStrategy - upkeep: Raise Dead (ghoul), Horn of Winter.
 *
 *  Each spec strategy registers cast::<spell> ActionNodes (with the keystone
 *  CastSpell family) + wires DK triggers; GetDefaultCombatActions is the spell
 *  priority list. Spell ids are resolved from NAMES at runtime (DB2-agnostic).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DK_STRATEGIES_H
#define PSYCHOBOT_DK_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Shared DK combat base (diseases / self-heal / interrupt / AoE).
    class GenericDKStrategy : public Strategy
    {
    public:
        GenericDKStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "dk"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // --- Blood (tank) ----------------------------------------------------
    class BloodDKStrategy : public GenericDKStrategy
    {
    public:
        BloodDKStrategy(PsychobotAI* ai) : GenericDKStrategy(ai) { }
        std::string GetName() const override { return "blood"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // --- Frost (dps) -----------------------------------------------------
    class FrostDKStrategy : public GenericDKStrategy
    {
    public:
        FrostDKStrategy(PsychobotAI* ai) : GenericDKStrategy(ai) { }
        std::string GetName() const override { return "frost"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // --- Unholy (dps) ----------------------------------------------------
    class UnholyDKStrategy : public GenericDKStrategy
    {
    public:
        UnholyDKStrategy(PsychobotAI* ai) : GenericDKStrategy(ai) { }
        std::string GetName() const override { return "unholy"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // --- non-combat upkeep (ghoul, horn of winter) -----------------------
    class GenericDKNonCombatStrategy : public Strategy
    {
    public:
        GenericDKNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "dk nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterDeathKnightStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_DK_STRATEGIES_H
