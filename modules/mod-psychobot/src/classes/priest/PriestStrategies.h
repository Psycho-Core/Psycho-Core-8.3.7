/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Priest STRATEGIES (S12) - Discipline / Holy heal + Shadow dps.
 *    GenericPriestStrategy - shared: Power Word: Shield a hurt ally, Desperate
 *                            Prayer self-save, Shadow Word: Death execute.
 *    DisciplinePriestStrategy - atonement healer: keep Atonement up (PWS /
 *                            Power Word: Radiance), Penance + Shadow Mend direct
 *                            heals, then Smite/Shadow Word: Pain to convert
 *                            damage into atonement healing; Pain Suppression CD.
 *    HolyPriestStrategy    - direct healer: Holy Word: Serenity (big single) +
 *                            Flash Heal / Heal + Prayer of Healing (AoE) +
 *                            Renew HoT; Smite filler; Guardian Spirit CD.
 *    ShadowPriestStrategy  - dps: Shadow Word: Pain + Vampiric Touch DoTs,
 *                            Mind Blast, Mind Flay filler, Devouring Plague
 *                            (Insanity spender), Void Eruption/Void Bolt,
 *                            Shadowfiend CD; Shadowform upkeep.
 *    GenericPriestNonCombatStrategy - Shadowform (Shadow) / Power Word: Fortitude.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PRIEST_STRATEGIES_H
#define PSYCHOBOT_PRIEST_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericPriestStrategy : public Strategy
    {
    public:
        GenericPriestStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "priest"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_RANGED; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class DisciplinePriestStrategy : public GenericPriestStrategy
    {
    public:
        DisciplinePriestStrategy(PsychobotAI* ai) : GenericPriestStrategy(ai) { }
        std::string GetName() const override { return "discipline"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_HEAL | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class HolyPriestStrategy : public GenericPriestStrategy
    {
    public:
        HolyPriestStrategy(PsychobotAI* ai) : GenericPriestStrategy(ai) { }
        std::string GetName() const override { return "holy"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_HEAL | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShadowPriestStrategy : public GenericPriestStrategy
    {
    public:
        ShadowPriestStrategy(PsychobotAI* ai) : GenericPriestStrategy(ai) { }
        std::string GetName() const override { return "shadow"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericPriestNonCombatStrategy : public Strategy
    {
    public:
        GenericPriestNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "priest nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterPriestStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_PRIEST_STRATEGIES_H
