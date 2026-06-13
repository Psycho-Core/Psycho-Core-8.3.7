/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Warrior rotations. Relevance scale (ACTION_NORMAL = 10): proc/Execute
 *  reactions and active mitigation sit above fillers; rage dumps above builders.
 *  All spells referenced by NAME (CastSpell family resolves the bot's known id).
 * ===========================================================================
 */

#include "WarriorStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const W_FILLER    = ACTION_NORMAL + 1;    // 11
    static float const W_SPEND     = ACTION_NORMAL + 2;    // 12 - rage dump
    static float const W_KEY       = ACTION_NORMAL + 3;    // 13 - signature strike
    static float const W_PROC      = ACTION_NORMAL + 4;    // 14 - proc reaction
    static float const W_EXECUTE   = ACTION_NORMAL + 5;    // 15 - Execute window
    static float const W_MITIGATE  = ACTION_NORMAL + 6;    // 16 - active mitigation
    static float const W_DEFENSIVE = ACTION_CRITICAL_HEAL; // 80 - defensive CD

    // ----------------------------------------------------------------------
    // GenericWarriorStrategy - shared (gap-closer + defensive CD).
    // ----------------------------------------------------------------------
    void GenericWarriorStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Charge the target when out of melee range.
        triggers.push_back(new TriggerNode("enemy out of melee",
            NextAction::Array(1, new NextAction("cast::Charge", ACTION_MOVE + 1), nullptr)));

        // Defensive CD when seriously hurt (Shield Wall for prot, Die by the
        // Sword for dps - both resolve only if known, so wiring both is safe).
        triggers.push_back(new TriggerNode("critical health",
            NextAction::Array(2,
                new NextAction("cast self::Shield Wall", W_DEFENSIVE),
                new NextAction("cast self::Die by the Sword", W_DEFENSIVE),
                nullptr)));
    }

    // ----------------------------------------------------------------------
    // Arms
    // ----------------------------------------------------------------------
    NextAction** ArmsWarriorStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast melee::Mortal Strike", W_KEY),
            new NextAction("cast melee::Slam",          W_FILLER),
            new NextAction("cast melee::Overpower",     W_FILLER + 1),
            new NextAction("attack",                    ACTION_NORMAL),
            nullptr);
    }

    void ArmsWarriorStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericWarriorStrategy::InitCombatTriggers(triggers);

        // Execute in the sub-20% window, or on a Sudden Death proc.
        triggers.push_back(new TriggerNode("target low health",
            NextAction::Array(1, new NextAction("cast melee::Execute", W_EXECUTE), nullptr)));
        triggers.push_back(new TriggerNode("sudden death",
            NextAction::Array(1, new NextAction("cast melee::Execute", W_PROC), nullptr)));

        // Overpower proc -> buffs Mortal Strike.
        triggers.push_back(new TriggerNode("overpower ready",
            NextAction::Array(1, new NextAction("cast melee::Overpower", W_PROC), nullptr)));

        // Colossus Smash CD opens the burst window.
        triggers.push_back(new TriggerNode("spell ready::Colossus Smash",
            NextAction::Array(1, new NextAction("cast melee::Colossus Smash", W_KEY + 1), nullptr)));

        // Dump rage with Slam.
        triggers.push_back(new TriggerNode("rage high",
            NextAction::Array(1, new NextAction("cast melee::Slam", W_SPEND), nullptr)));

        // AoE: Whirlwind.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Whirlwind", ACTION_NORMAL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Fury
    // ----------------------------------------------------------------------
    NextAction** FuryWarriorStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast melee::Bloodthirst", W_KEY),
            new NextAction("cast melee::Raging Blow", W_FILLER + 1),
            new NextAction("cast melee::Rampage",     W_SPEND),
            new NextAction("attack",                  ACTION_NORMAL),
            nullptr);
    }

    void FuryWarriorStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericWarriorStrategy::InitCombatTriggers(triggers);

        // Rampage at high rage to gain/refresh Enrage.
        triggers.push_back(new TriggerNode("rage high",
            NextAction::Array(1, new NextAction("cast melee::Rampage", W_SPEND + 1), nullptr)));
        triggers.push_back(new TriggerNode("enrage absent",
            NextAction::Array(1, new NextAction("cast melee::Rampage", W_KEY), nullptr)));

        // Execute window / Sudden Death proc.
        triggers.push_back(new TriggerNode("target low health",
            NextAction::Array(1, new NextAction("cast melee::Execute", W_EXECUTE), nullptr)));
        triggers.push_back(new TriggerNode("sudden death",
            NextAction::Array(1, new NextAction("cast melee::Execute", W_PROC), nullptr)));

        // Recklessness burst CD.
        triggers.push_back(new TriggerNode("spell ready::Recklessness",
            NextAction::Array(1, new NextAction("cast self::Recklessness", W_KEY + 1), nullptr)));

        // AoE: Whirlwind (cleave).
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Whirlwind", ACTION_NORMAL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Protection
    // ----------------------------------------------------------------------
    NextAction** ProtectionWarriorStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast melee::Shield Slam", W_KEY),
            new NextAction("cast melee::Revenge",     W_FILLER + 1),
            new NextAction("cast melee::Devastate",   W_FILLER),
            new NextAction("attack",                  ACTION_NORMAL),
            nullptr);
    }

    void ProtectionWarriorStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericWarriorStrategy::InitCombatTriggers(triggers);

        // Shield Slam on cooldown (rage gen + threat).
        triggers.push_back(new TriggerNode("spell ready::Shield Slam",
            NextAction::Array(1, new NextAction("cast melee::Shield Slam", W_KEY), nullptr)));

        // Active mitigation: keep Shield Block up; Ignore Pain to spend rage.
        triggers.push_back(new TriggerNode("shield block absent",
            NextAction::Array(1, new NextAction("cast self::Shield Block", W_MITIGATE), nullptr)));
        triggers.push_back(new TriggerNode("rage high",
            NextAction::Array(1, new NextAction("cast self::Ignore Pain", W_SPEND), nullptr)));

        // AoE: Thunder Clap when packs are close.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Thunder Clap", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: stance upkeep
    // ----------------------------------------------------------------------
    void GenericWarriorNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Get into the right stance. Both are wired; only the known/missing one
        // fires for a given spec (the trigger gates on the stance aura missing).
        triggers.push_back(new TriggerNode("battle stance absent",
            NextAction::Array(1, new NextAction("cast self::Battle Stance", ACTION_IDLE + 1), nullptr)));
        triggers.push_back(new TriggerNode("defensive stance absent",
            NextAction::Array(1, new NextAction("cast self::Defensive Stance", ACTION_IDLE + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class WarriorStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            WarriorStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["warrior"]    = [](PsychobotAI* ai) -> Strategy* { return new GenericWarriorStrategy(ai); };
                _creators["arms"]       = [](PsychobotAI* ai) -> Strategy* { return new ArmsWarriorStrategy(ai); };
                _creators["fury"]       = [](PsychobotAI* ai) -> Strategy* { return new FuryWarriorStrategy(ai); };
                _creators["protection"] = [](PsychobotAI* ai) -> Strategy* { return new ProtectionWarriorStrategy(ai); };
                _creators["warrior nc"] = [](PsychobotAI* ai) -> Strategy* { return new GenericWarriorNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterWarriorStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new WarriorStrategyContext());
    }
}
