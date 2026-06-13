/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Demon Hunter rotations (NEW class, authored from research). Relevance
 *  scale (ACTION_NORMAL = 10). Builder/spender: generators build Fury/Pain +
 *  Soul Fragments, spenders consume them. Tank mitigation (Demon Spikes/Fiery
 *  Brand) sits above fillers. All spells referenced by NAME.
 * ===========================================================================
 */

#include "DemonHunterStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const H_BUILD = ACTION_NORMAL + 1;     // 11 - resource builders
    static float const H_SPEND = ACTION_NORMAL + 3;     // 13 - resource spenders
    static float const H_KEY   = ACTION_NORMAL + 4;     // 14 - signature
    static float const H_MITI  = ACTION_NORMAL + 5;     // 15 - tank mitigation
    static float const H_CD    = ACTION_NORMAL + 2;     // 12 - offensive CDs
    static float const H_DEFEND= ACTION_CRITICAL_HEAL;  // 80 - defensive CD

    // ----------------------------------------------------------------------
    // GenericDemonHunterStrategy - shared (interrupt + gap-closer + Meta).
    // ----------------------------------------------------------------------
    void GenericDemonHunterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Consume Magic interrupt on a casting target.
        triggers.push_back(new TriggerNode("target casting",
            NextAction::Array(1, new NextAction("cast::Consume Magic", ACTION_INTERRUPT), nullptr)));

        // Fel Rush to close distance when out of melee.
        triggers.push_back(new TriggerNode("enemy out of melee",
            NextAction::Array(1, new NextAction("cast::Fel Rush", ACTION_MOVE + 1), nullptr)));

        // Immolation Aura on cooldown (Fury/Pain gen + AoE; both specs).
        triggers.push_back(new TriggerNode("immolation aura absent",
            NextAction::Array(1, new NextAction("cast self::Immolation Aura", H_BUILD), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Havoc (dps)
    // ----------------------------------------------------------------------
    NextAction** HavocDemonHunterStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(3,
            new NextAction("cast melee::Chaos Strike", H_SPEND),
            new NextAction("cast melee::Demon's Bite", H_BUILD),
            new NextAction("attack",                   ACTION_NORMAL),
            nullptr);
    }

    void HavocDemonHunterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDemonHunterStrategy::InitCombatTriggers(triggers);

        // Eye Beam on cooldown (channel burst).
        triggers.push_back(new TriggerNode("spell ready::Eye Beam",
            NextAction::Array(1, new NextAction("cast::Eye Beam", H_KEY), nullptr)));

        // Spend Fury on Chaos Strike when high.
        triggers.push_back(new TriggerNode("fury high",
            NextAction::Array(1, new NextAction("cast melee::Chaos Strike", H_SPEND), nullptr)));

        // Metamorphosis burst CD.
        triggers.push_back(new TriggerNode("spell ready::Metamorphosis",
            NextAction::Array(1, new NextAction("cast self::Metamorphosis", H_CD), nullptr)));

        // AoE: Blade Dance.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Blade Dance", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Vengeance (tank)
    // ----------------------------------------------------------------------
    NextAction** VengeanceDemonHunterStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(3,
            new NextAction("cast melee::Fracture",    H_BUILD),
            new NextAction("cast melee::Soul Cleave", H_SPEND),
            new NextAction("attack",                  ACTION_NORMAL),
            nullptr);
    }

    void VengeanceDemonHunterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDemonHunterStrategy::InitCombatTriggers(triggers);

        // Demon Spikes mitigation; Fiery Brand defensive when hurt.
        triggers.push_back(new TriggerNode("demon spikes absent",
            NextAction::Array(1, new NextAction("cast self::Demon Spikes", H_MITI), nullptr)));
        triggers.push_back(new TriggerNode("critical health",
            NextAction::Array(1, new NextAction("cast::Fiery Brand", H_DEFEND), nullptr)));

        // Spend Soul Fragments on Spirit Bomb (AoE/Frailty) when high.
        triggers.push_back(new TriggerNode("soul fragments high",
            NextAction::Array(1, new NextAction("cast self::Spirit Bomb", H_SPEND + 1), nullptr)));

        // Spend Pain on Soul Cleave (damage + self-heal) when high.
        triggers.push_back(new TriggerNode("pain high",
            NextAction::Array(1, new NextAction("cast melee::Soul Cleave", H_SPEND), nullptr)));

        // Sigil of Flame on cooldown (Pain gen + AoE).
        triggers.push_back(new TriggerNode("spell ready::Sigil of Flame",
            NextAction::Array(1, new NextAction("cast self::Sigil of Flame", H_BUILD), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat (placeholder hook; DH has no major out-of-combat upkeep)
    // ----------------------------------------------------------------------
    void GenericDemonHunterNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& /*triggers*/)
    {
        // Demon Hunters have no standing buff to maintain out of combat in 8.3;
        // the hook exists so the non-combat engine has a class strategy slot.
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class DHStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            DHStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["demon hunter"]    = [](PsychobotAI* ai) -> Strategy* { return new GenericDemonHunterStrategy(ai); };
                _creators["havoc"]           = [](PsychobotAI* ai) -> Strategy* { return new HavocDemonHunterStrategy(ai); };
                _creators["vengeance"]       = [](PsychobotAI* ai) -> Strategy* { return new VengeanceDemonHunterStrategy(ai); };
                _creators["demon hunter nc"] = [](PsychobotAI* ai) -> Strategy* { return new GenericDemonHunterNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterDemonHunterStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new DHStrategyContext());
    }
}
