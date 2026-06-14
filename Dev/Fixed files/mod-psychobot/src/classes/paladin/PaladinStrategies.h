/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Paladin STRATEGIES (S9) - Holy / Protection / Retribution + blessings/auras.
 *    GenericPaladinStrategy - shared: Hammer of Wrath execute, Lay on Hands /
 *                             Divine Shield emergency self-save.
 *    HolyPaladinStrategy    - healer: Holy Shock + Light of Dawn (AoE, 3 HP) +
 *                             Word of Glory (3 HP) + Flash/Holy Light heals on
 *                             the party member to heal; Judgment/Crusader Strike
 *                             when nobody needs healing.
 *    ProtectionPaladinStrategy - tank: Avenger's Shield, Judgment, Hammer of the
 *                             Righteous, Consecration AoE, Shield of the
 *                             Righteous mitigation, Word of Glory self-heal.
 *    RetributionPaladinStrategy - dps: Crusader Strike + Judgment + Blade of
 *                             Justice (HP gen), Templar's Verdict (3 HP),
 *                             Divine Storm AoE, Avenging Wrath CD.
 *    GenericPaladinNonCombatStrategy - Devotion Aura + Blessing of Kings upkeep.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PALADIN_STRATEGIES_H
#define PSYCHOBOT_PALADIN_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericPaladinStrategy : public Strategy
    {
    public:
        GenericPaladinStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "paladin"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class HolyPaladinStrategy : public GenericPaladinStrategy
    {
    public:
        HolyPaladinStrategy(PsychobotAI* ai) : GenericPaladinStrategy(ai) { }
        std::string GetName() const override { return "holy"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_HEAL | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ProtectionPaladinStrategy : public GenericPaladinStrategy
    {
    public:
        ProtectionPaladinStrategy(PsychobotAI* ai) : GenericPaladinStrategy(ai) { }
        std::string GetName() const override { return "protection"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RetributionPaladinStrategy : public GenericPaladinStrategy
    {
    public:
        RetributionPaladinStrategy(PsychobotAI* ai) : GenericPaladinStrategy(ai) { }
        std::string GetName() const override { return "retribution"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericPaladinNonCombatStrategy : public Strategy
    {
    public:
        GenericPaladinNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "paladin nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterPaladinStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_PALADIN_STRATEGIES_H
