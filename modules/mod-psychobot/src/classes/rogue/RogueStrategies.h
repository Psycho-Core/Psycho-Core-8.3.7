/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Rogue STRATEGIES (S11) - Assassination / Outlaw / Subtlety + stealth/combo
 *  points/energy. All three are melee combo-point specs: BUILDERS generate
 *  combo points, FINISHERS spend >=5. The shared logic gates finishers behind
 *  "combo points full" so the bot builds then spends, the way the game plays.
 *    GenericRogueStrategy - shared: Crimson Vial self-heal, Kick interrupt,
 *      Stealth opener wiring (Cheap Shot from stealth).
 *    AssassinationRogueStrategy - Mutilate (builder) -> Envenom (finisher);
 *      Garrote + Rupture bleeds; Fan of Knives AoE; Vendetta CD.
 *    OutlawRogueStrategy - Sinister Strike/Pistol Shot (builders) -> Dispatch
 *      (finisher); Roll the Bones + Slice and Dice; Blade Flurry AoE;
 *      Adrenaline Rush CD.
 *    SubtletyRogueStrategy - Shadowstrike/Backstab (builders) -> Eviscerate
 *      (finisher); Nightblade bleed; Symbols of Death + Shadow Dance CDs;
 *      Shuriken Storm AoE.
 *    GenericRogueNonCombatStrategy - Stealth + Slice and Dice upkeep.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ROGUE_STRATEGIES_H
#define PSYCHOBOT_ROGUE_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericRogueStrategy : public Strategy
    {
    public:
        GenericRogueStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "rogue"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class AssassinationRogueStrategy : public GenericRogueStrategy
    {
    public:
        AssassinationRogueStrategy(PsychobotAI* ai) : GenericRogueStrategy(ai) { }
        std::string GetName() const override { return "assassination"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class OutlawRogueStrategy : public GenericRogueStrategy
    {
    public:
        OutlawRogueStrategy(PsychobotAI* ai) : GenericRogueStrategy(ai) { }
        std::string GetName() const override { return "outlaw"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class SubtletyRogueStrategy : public GenericRogueStrategy
    {
    public:
        SubtletyRogueStrategy(PsychobotAI* ai) : GenericRogueStrategy(ai) { }
        std::string GetName() const override { return "subtlety"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericRogueNonCombatStrategy : public Strategy
    {
    public:
        GenericRogueNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "rogue nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterRogueStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_ROGUE_STRATEGIES_H
