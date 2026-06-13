/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Warlock rotations. Relevance scale (ACTION_NORMAL = 10). Pet upkeep
 *  outranks the rotation (a Warlock needs its demon); DoT upkeep above fillers;
 *  Soul-Shard spenders (Chaos Bolt / Unstable Affliction / Hand of Gul'dan) fire
 *  at high shards; proc reactions (Demonic Core) above fillers. All by NAME.
 * ===========================================================================
 */

#include "WarlockStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const L_FILLER = ACTION_NORMAL + 1;     // 11 - shard builders
    static float const L_DOT    = ACTION_NORMAL + 3;     // 13 - DoT upkeep
    static float const L_SPEND  = ACTION_NORMAL + 4;     // 14 - shard spenders
    static float const L_PROC   = ACTION_NORMAL + 5;     // 15 - proc reaction
    static float const L_CD     = ACTION_NORMAL + 2;     // 12 - offensive CDs
    static float const L_PET    = ACTION_HIGH + 5;       // 25 - summon pet
    static float const L_SAVE   = ACTION_EMERGENCY;      // 90 - Unending Resolve

    // ----------------------------------------------------------------------
    // GenericWarlockStrategy - shared (self-heal + defensive + Fear).
    // ----------------------------------------------------------------------
    void GenericWarlockStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Drain Life self-heal when hurt.
        triggers.push_back(new TriggerNode("low health",
            NextAction::Array(1, new NextAction("cast::Drain Life", ACTION_MEDIUM_HEAL), nullptr)));

        // Unending Resolve when critical.
        triggers.push_back(new TriggerNode("critical health",
            NextAction::Array(1, new NextAction("cast self::Unending Resolve", L_SAVE), nullptr)));

        // Fear an enemy that reached melee, then kite.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Fear", ACTION_MOVE), nullptr)));

        // No demon in combat -> summon one (all specs want their pet).
        triggers.push_back(new TriggerNode("no warlock pet",
            NextAction::Array(1, new NextAction("cast self::Summon Imp", L_PET), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Affliction
    // ----------------------------------------------------------------------
    NextAction** AfflictionWarlockStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast::Drain Soul", L_FILLER),
            new NextAction("attack",           ACTION_NORMAL),
            nullptr);
    }

    void AfflictionWarlockStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericWarlockStrategy::InitCombatTriggers(triggers);

        // Keep the DoTs up.
        triggers.push_back(new TriggerNode("agony absent",
            NextAction::Array(1, new NextAction("cast::Agony", L_DOT + 1), nullptr)));
        triggers.push_back(new TriggerNode("corruption absent",
            NextAction::Array(1, new NextAction("cast::Corruption", L_DOT), nullptr)));

        // Spend shards on Unstable Affliction.
        triggers.push_back(new TriggerNode("soul shards high",
            NextAction::Array(1, new NextAction("cast::Unstable Affliction", L_SPEND), nullptr)));

        // Summon Darkglare burst CD.
        triggers.push_back(new TriggerNode("spell ready::Summon Darkglare",
            NextAction::Array(1, new NextAction("cast self::Summon Darkglare", L_CD), nullptr)));

        // AoE: Seed of Corruption.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Seed of Corruption", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Demonology
    // ----------------------------------------------------------------------
    NextAction** DemonologyWarlockStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast::Shadow Bolt", L_FILLER),
            new NextAction("attack",            ACTION_NORMAL),
            nullptr);
    }

    void DemonologyWarlockStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericWarlockStrategy::InitCombatTriggers(triggers);

        // Demonic Core proc -> instant Demonbolt.
        triggers.push_back(new TriggerNode("demonic core",
            NextAction::Array(1, new NextAction("cast::Demonbolt", L_PROC), nullptr)));

        // Call Dreadstalkers on cooldown.
        triggers.push_back(new TriggerNode("spell ready::Call Dreadstalkers",
            NextAction::Array(1, new NextAction("cast::Call Dreadstalkers", L_SPEND), nullptr)));

        // Spend shards on Hand of Gul'dan (summons Wild Imps).
        triggers.push_back(new TriggerNode("soul shards high",
            NextAction::Array(1, new NextAction("cast::Hand of Gul'dan", L_SPEND), nullptr)));

        // Summon Demonic Tyrant burst CD.
        triggers.push_back(new TriggerNode("spell ready::Summon Demonic Tyrant",
            NextAction::Array(1, new NextAction("cast self::Summon Demonic Tyrant", L_CD), nullptr)));

        // AoE: Implosion.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast self::Implosion", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Destruction
    // ----------------------------------------------------------------------
    NextAction** DestructionWarlockStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast::Incinerate", L_FILLER),
            new NextAction("attack",           ACTION_NORMAL),
            nullptr);
    }

    void DestructionWarlockStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericWarlockStrategy::InitCombatTriggers(triggers);

        // Keep Immolate up.
        triggers.push_back(new TriggerNode("immolate absent",
            NextAction::Array(1, new NextAction("cast::Immolate", L_DOT), nullptr)));

        // Conflagrate on cooldown (instant, builds shards).
        triggers.push_back(new TriggerNode("spell ready::Conflagrate",
            NextAction::Array(1, new NextAction("cast::Conflagrate", L_FILLER + 1), nullptr)));

        // Spend shards on Chaos Bolt.
        triggers.push_back(new TriggerNode("soul shards high",
            NextAction::Array(1, new NextAction("cast::Chaos Bolt", L_SPEND), nullptr)));

        // Summon Infernal burst CD.
        triggers.push_back(new TriggerNode("spell ready::Summon Infernal",
            NextAction::Array(1, new NextAction("cast self::Summon Infernal", L_CD), nullptr)));

        // AoE: Rain of Fire.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Rain of Fire", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: summon pet + healthstone
    // ----------------------------------------------------------------------
    void GenericWarlockNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Summon a demon if we have none (Imp is the safe default).
        triggers.push_back(new TriggerNode("no warlock pet",
            NextAction::Array(1, new NextAction("cast self::Summon Imp", ACTION_IDLE + 2), nullptr)));

        // Create Healthstone when off cooldown.
        triggers.push_back(new TriggerNode("spell ready::Create Healthstone",
            NextAction::Array(1, new NextAction("cast self::Create Healthstone", ACTION_IDLE), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class WarlockStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            WarlockStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["warlock"]     = [](PsychobotAI* ai) -> Strategy* { return new GenericWarlockStrategy(ai); };
                _creators["affliction"]  = [](PsychobotAI* ai) -> Strategy* { return new AfflictionWarlockStrategy(ai); };
                _creators["demonology"]  = [](PsychobotAI* ai) -> Strategy* { return new DemonologyWarlockStrategy(ai); };
                _creators["destruction"] = [](PsychobotAI* ai) -> Strategy* { return new DestructionWarlockStrategy(ai); };
                _creators["warlock nc"]  = [](PsychobotAI* ai) -> Strategy* { return new GenericWarlockNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterWarlockStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new WarlockStrategyContext());
    }
}
