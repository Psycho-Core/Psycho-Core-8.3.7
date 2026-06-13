/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Monk rotations (NEW class, authored from research). Relevance scale
 *  (ACTION_NORMAL = 10). Tank mitigation (Ironskin/Purifying Brew) and Chi
 *  spenders sit above fillers; heals use ACTION_*_HEAL tiers. All spells by NAME.
 * ===========================================================================
 */

#include "MonkStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const K_FILLER = ACTION_NORMAL + 1;     // 11 - Chi/energy fillers
    static float const K_KEY    = ACTION_NORMAL + 3;     // 13 - signature strikes
    static float const K_SPEND  = ACTION_NORMAL + 4;     // 14 - Chi spenders
    static float const K_MITI   = ACTION_NORMAL + 5;     // 15 - brew mitigation
    static float const K_CD     = ACTION_NORMAL + 2;     // 12 - offensive CDs
    static float const K_EXEC   = ACTION_NORMAL + 6;     // 16 - Touch of Death
    static float const K_EMERG  = ACTION_CRITICAL_HEAL;  // 80 - Expel Harm

    // ----------------------------------------------------------------------
    // GenericMonkStrategy - shared (self-heal + interrupt + execute).
    // ----------------------------------------------------------------------
    void GenericMonkStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Expel Harm self-heal when hurt.
        triggers.push_back(new TriggerNode("low health",
            NextAction::Array(1, new NextAction("cast self::Expel Harm", K_EMERG), nullptr)));

        // Spear Hand Strike interrupt on a casting target.
        triggers.push_back(new TriggerNode("target casting",
            NextAction::Array(1, new NextAction("cast melee::Spear Hand Strike", ACTION_INTERRUPT), nullptr)));

        // Touch of Death execute.
        triggers.push_back(new TriggerNode("target low health",
            NextAction::Array(1, new NextAction("cast melee::Touch of Death", K_EXEC), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Brewmaster (tank)
    // ----------------------------------------------------------------------
    NextAction** BrewmasterMonkStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast melee::Keg Smash",       K_KEY),
            new NextAction("cast melee::Blackout Strike", K_FILLER + 1),
            new NextAction("cast melee::Tiger Palm",      K_FILLER),
            new NextAction("attack",                      ACTION_NORMAL),
            nullptr);
    }

    void BrewmasterMonkStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericMonkStrategy::InitCombatTriggers(triggers);

        // Keep Ironskin Brew up (stagger mitigation); keep Shuffle via Keg Smash.
        triggers.push_back(new TriggerNode("ironskin brew absent",
            NextAction::Array(1, new NextAction("cast self::Ironskin Brew", K_MITI), nullptr)));
        triggers.push_back(new TriggerNode("shuffle absent",
            NextAction::Array(1, new NextAction("cast melee::Keg Smash", K_MITI - 1), nullptr)));

        // Purify staggered damage when seriously hurt.
        triggers.push_back(new TriggerNode("low health",
            NextAction::Array(1, new NextAction("cast self::Purifying Brew", ACTION_MEDIUM_HEAL), nullptr)));

        // Keg Smash on cooldown (threat + Shuffle).
        triggers.push_back(new TriggerNode("spell ready::Keg Smash",
            NextAction::Array(1, new NextAction("cast melee::Keg Smash", K_KEY), nullptr)));

        // Breath of Fire on cooldown (AoE DoT).
        triggers.push_back(new TriggerNode("spell ready::Breath of Fire",
            NextAction::Array(1, new NextAction("cast::Breath of Fire", K_FILLER + 1), nullptr)));

        // AoE: Spinning Crane Kick.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Spinning Crane Kick", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Mistweaver (healer)
    // ----------------------------------------------------------------------
    NextAction** MistweaverMonkStrategy::GetDefaultCombatActions()
    {
        // Fistweave damage when nobody needs healing.
        return NextAction::Array(2,
            new NextAction("cast melee::Tiger Palm", K_FILLER),
            new NextAction("attack",                 ACTION_NORMAL),
            nullptr);
    }

    void MistweaverMonkStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericMonkStrategy::InitCombatTriggers(triggers);

        // Renewing Mist (HoT) + Vivify (fast) + Enveloping Mist (strong) tiers.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Renewing Mist", ACTION_LIGHT_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Vivify", ACTION_MEDIUM_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Enveloping Mist", ACTION_CRITICAL_HEAL), nullptr)));

        // Essence Font for group damage.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Essence Font", ACTION_MEDIUM_HEAL - 1), nullptr)));

        // S21: cure a debuffed ally (Detox removes Magic/Poison/Disease).
        triggers.push_back(new TriggerNode("party member needs dispel",
            NextAction::Array(1, new NextAction("cast dispel::Detox", ACTION_DISPEL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Windwalker (dps)
    // ----------------------------------------------------------------------
    NextAction** WindwalkerMonkStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(3,
            new NextAction("cast melee::Tiger Palm",     K_FILLER),
            new NextAction("cast melee::Blackout Kick",  K_SPEND - 1),
            new NextAction("attack",                     ACTION_NORMAL),
            nullptr);
    }

    void WindwalkerMonkStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericMonkStrategy::InitCombatTriggers(triggers);

        // Rising Sun Kick on cooldown (key Chi spender).
        triggers.push_back(new TriggerNode("spell ready::Rising Sun Kick",
            NextAction::Array(1, new NextAction("cast melee::Rising Sun Kick", K_KEY), nullptr)));

        // Spend Chi on Blackout Kick / Rising Sun Kick when high.
        triggers.push_back(new TriggerNode("chi high",
            NextAction::Array(1, new NextAction("cast melee::Blackout Kick", K_SPEND), nullptr)));

        // Fists of Fury burst CD.
        triggers.push_back(new TriggerNode("spell ready::Fists of Fury",
            NextAction::Array(1, new NextAction("cast melee::Fists of Fury", K_CD), nullptr)));

        // AoE: Spinning Crane Kick.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Spinning Crane Kick", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: stat buff upkeep
    // ----------------------------------------------------------------------
    void GenericMonkNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Mystic Touch / stat buff (BfA: Legacy of the Emperor became passive,
        // but cast it if known + missing as a safe upkeep).
        triggers.push_back(new TriggerNode("no aura::Legacy of the Emperor",
            NextAction::Array(1, new NextAction("cast buff::Legacy of the Emperor", ACTION_IDLE), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class MonkStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            MonkStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["monk"]       = [](PsychobotAI* ai) -> Strategy* { return new GenericMonkStrategy(ai); };
                _creators["brewmaster"] = [](PsychobotAI* ai) -> Strategy* { return new BrewmasterMonkStrategy(ai); };
                _creators["mistweaver"] = [](PsychobotAI* ai) -> Strategy* { return new MistweaverMonkStrategy(ai); };
                _creators["windwalker"] = [](PsychobotAI* ai) -> Strategy* { return new WindwalkerMonkStrategy(ai); };
                _creators["monk nc"]    = [](PsychobotAI* ai) -> Strategy* { return new GenericMonkNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterMonkStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new MonkStrategyContext());
    }
}
