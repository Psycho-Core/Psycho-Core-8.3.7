/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Warlock STRATEGIES (S15) - Affliction / Demonology / Destruction + pet /
 *  soul shards / DoTs.
 *    GenericWarlockStrategy - shared: Drain Life self-heal, Unending Resolve
 *                             defensive, Fear an enemy in melee.
 *    AfflictionWarlockStrategy - DoTs: Agony + Corruption + Unstable Affliction
 *                             (shard spender), Malefic Rapture / Haunt, Summon
 *                             Darkglare CD, Drain Soul filler/execute.
 *    DemonologyWarlockStrategy - pet army: Call Dreadstalkers, Hand of Gul'dan
 *                             (3 shards), Demonbolt on Demonic Core, Shadow Bolt
 *                             builder, Summon Demonic Tyrant CD, Implosion AoE.
 *    DestructionWarlockStrategy - burst: Immolate DoT, Incinerate filler,
 *                             Conflagrate, Chaos Bolt (shard spender), Summon
 *                             Infernal CD, Rain of Fire AoE.
 *    GenericWarlockNonCombatStrategy - summon pet (Imp) + Create Healthstone.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_WARLOCK_STRATEGIES_H
#define PSYCHOBOT_WARLOCK_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericWarlockStrategy : public Strategy
    {
    public:
        GenericWarlockStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "warlock"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_RANGED; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class AfflictionWarlockStrategy : public GenericWarlockStrategy
    {
    public:
        AfflictionWarlockStrategy(PsychobotAI* ai) : GenericWarlockStrategy(ai) { }
        std::string GetName() const override { return "affliction"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class DemonologyWarlockStrategy : public GenericWarlockStrategy
    {
    public:
        DemonologyWarlockStrategy(PsychobotAI* ai) : GenericWarlockStrategy(ai) { }
        std::string GetName() const override { return "demonology"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class DestructionWarlockStrategy : public GenericWarlockStrategy
    {
    public:
        DestructionWarlockStrategy(PsychobotAI* ai) : GenericWarlockStrategy(ai) { }
        std::string GetName() const override { return "destruction"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericWarlockNonCombatStrategy : public Strategy
    {
    public:
        GenericWarlockNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "warlock nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterWarlockStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_WARLOCK_STRATEGIES_H
