/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Generic STRATEGIES (S6). These are the class-agnostic behaviour packs every
 *  bot runs; per-class spec strategies (S7+) ADD their rotation triggers on top.
 *
 *    "follow"  (non-combat) - default action: follow the master / stay close.
 *    "melee"   (combat)     - reach melee then auto-attack the current target.
 *    "ranged"  (combat)     - keep range and auto-attack (placeholder until the
 *                             class rotation supplies real ranged casts).
 *    "tank"    (combat)     - melee + (later) threat/taunt wiring.
 *    "heal"    (combat)     - (placeholder) react to "party member low health".
 *    "dead"    (dead)       - (placeholder) release/resurrect handled later.
 *
 *  Wiring uses TriggerNode + NextAction::Array exactly like the reference, so
 *  class strategies extend this seamlessly.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_STRATEGIES_GENERICSTRATEGIES_H
#define PSYCHOBOT_STRATEGIES_GENERICSTRATEGIES_H

#include "../engine/Strategy.h"
#include "../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // --- non-combat: follow master ---------------------------------------
    class FollowStrategy : public Strategy
    {
    public:
        FollowStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "follow"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        NextAction** GetDefaultNonCombatActions() override;
    };

    // --- non-combat: rest / repair (S22 world housekeeping) ----------------
    class RestStrategy : public Strategy
    {
    public:
        RestStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "rest"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        NextAction** GetDefaultNonCombatActions() override;
    };

    // --- combat: melee ----------------------------------------------------
    class MeleeCombatStrategy : public Strategy
    {
    public:
        MeleeCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "melee"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // --- combat: ranged (auto-attack fallback; class casts added in S7+) ---
    class RangedCombatStrategy : public Strategy
    {
    public:
        RangedCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "ranged"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // --- combat: tank (melee + threat hooks later) ------------------------
    class TankCombatStrategy : public MeleeCombatStrategy
    {
    public:
        TankCombatStrategy(PsychobotAI* ai) : MeleeCombatStrategy(ai) { }
        std::string GetName() const override { return "tank"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }
    };

    // --- combat: heal (placeholder until S21 healing depth) ---------------
    class HealStrategy : public Strategy
    {
    public:
        HealStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "heal"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_HEAL; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // Register the generic strategies into the bot's base strategy context.
    void RegisterGenericStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_STRATEGIES_GENERICSTRATEGIES_H
