/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Rogue rotations. Relevance scale (ACTION_NORMAL = 10). FINISHERS are
 *  gated by "combo points full" so they only fire at >=5 CP and outrank the
 *  builders; bleeds/buff upkeep sit above builders; builders are the default
 *  fillers. All spells referenced by NAME (CastSpell family resolves the id).
 * ===========================================================================
 */

#include "RogueStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const R_BUILDER  = ACTION_NORMAL + 1;     // 11 - combo builders
    static float const R_BLEED    = ACTION_NORMAL + 3;     // 13 - bleed/DoT upkeep
    static float const R_FINISH   = ACTION_NORMAL + 5;     // 15 - >=5 CP finisher
    static float const R_OPENER   = ACTION_HIGH + 2;       // 22 - stealth opener
    static float const R_CD       = ACTION_NORMAL + 4;     // 14 - offensive CDs
    static float const R_EMERGENCY= ACTION_CRITICAL_HEAL;  // 80 - Crimson Vial

    // ----------------------------------------------------------------------
    // GenericRogueStrategy - shared (self-heal + interrupt + stealth opener).
    // ----------------------------------------------------------------------
    void GenericRogueStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Crimson Vial self-heal when hurt.
        triggers.push_back(new TriggerNode("low health",
            NextAction::Array(1, new NextAction("cast self::Crimson Vial", R_EMERGENCY), nullptr)));

        // Interrupt a casting target.
        triggers.push_back(new TriggerNode("target casting",
            NextAction::Array(1, new NextAction("cast melee::Kick", ACTION_INTERRUPT), nullptr)));

        // Stealth opener: Cheap Shot when stealthed (stun + opener).
        triggers.push_back(new TriggerNode("stealthed",
            NextAction::Array(1, new NextAction("cast melee::Cheap Shot", R_OPENER), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Assassination
    // ----------------------------------------------------------------------
    NextAction** AssassinationRogueStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast melee::Mutilate", R_BUILDER),
            new NextAction("attack",               ACTION_NORMAL),
            nullptr);
    }

    void AssassinationRogueStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericRogueStrategy::InitCombatTriggers(triggers);

        // Finisher at >=5 combo points.
        triggers.push_back(new TriggerNode("combo points full",
            NextAction::Array(1, new NextAction("cast melee::Envenom", R_FINISH), nullptr)));

        // Keep the two bleeds up.
        triggers.push_back(new TriggerNode("garrote absent",
            NextAction::Array(1, new NextAction("cast melee::Garrote", R_BLEED + 1), nullptr)));
        triggers.push_back(new TriggerNode("rupture absent",
            NextAction::Array(1, new NextAction("cast melee::Rupture", R_BLEED), nullptr)));

        // Vendetta burst CD.
        triggers.push_back(new TriggerNode("spell ready::Vendetta",
            NextAction::Array(1, new NextAction("cast::Vendetta", R_CD), nullptr)));

        // AoE: Fan of Knives.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Fan of Knives", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Outlaw
    // ----------------------------------------------------------------------
    NextAction** OutlawRogueStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast melee::Sinister Strike", R_BUILDER),
            new NextAction("attack",                      ACTION_NORMAL),
            nullptr);
    }

    void OutlawRogueStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericRogueStrategy::InitCombatTriggers(triggers);

        // Finisher at >=5 combo points.
        triggers.push_back(new TriggerNode("combo points full",
            NextAction::Array(1, new NextAction("cast melee::Dispatch", R_FINISH), nullptr)));

        // Keep Roll the Bones / Slice and Dice buffs up.
        triggers.push_back(new TriggerNode("slice and dice absent",
            NextAction::Array(2,
                new NextAction("cast self::Roll the Bones", R_BLEED),
                new NextAction("cast self::Slice and Dice", R_BLEED - 1),
                nullptr)));

        // Adrenaline Rush burst CD.
        triggers.push_back(new TriggerNode("spell ready::Adrenaline Rush",
            NextAction::Array(1, new NextAction("cast self::Adrenaline Rush", R_CD), nullptr)));

        // AoE: Blade Flurry on + Sinister Strike cleaves.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast self::Blade Flurry", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Subtlety
    // ----------------------------------------------------------------------
    NextAction** SubtletyRogueStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast melee::Backstab", R_BUILDER),
            new NextAction("attack",               ACTION_NORMAL),
            nullptr);
    }

    void SubtletyRogueStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericRogueStrategy::InitCombatTriggers(triggers);

        // Shadowstrike when stealthed/Shadow Dance (better builder).
        triggers.push_back(new TriggerNode("stealthed",
            NextAction::Array(1, new NextAction("cast melee::Shadowstrike", R_OPENER + 1), nullptr)));

        // Finisher at >=5 combo points.
        triggers.push_back(new TriggerNode("combo points full",
            NextAction::Array(1, new NextAction("cast melee::Eviscerate", R_FINISH), nullptr)));

        // Keep Nightblade bleed up.
        triggers.push_back(new TriggerNode("rupture absent",
            NextAction::Array(1, new NextAction("cast melee::Nightblade", R_BLEED), nullptr)));

        // Symbols of Death + Shadow Dance CDs.
        triggers.push_back(new TriggerNode("spell ready::Symbols of Death",
            NextAction::Array(1, new NextAction("cast self::Symbols of Death", R_CD), nullptr)));
        triggers.push_back(new TriggerNode("spell ready::Shadow Dance",
            NextAction::Array(1, new NextAction("cast self::Shadow Dance", R_CD), nullptr)));

        // AoE: Shuriken Storm.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Shuriken Storm", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: stealth + slice and dice upkeep
    // ----------------------------------------------------------------------
    void GenericRogueNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Go into Stealth when not stealthed and out of combat.
        triggers.push_back(new TriggerNode("not stealthed",
            NextAction::Array(1, new NextAction("cast self::Stealth", ACTION_IDLE + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class RogueStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            RogueStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["rogue"]          = [](PsychobotAI* ai) -> Strategy* { return new GenericRogueStrategy(ai); };
                _creators["assassination"]  = [](PsychobotAI* ai) -> Strategy* { return new AssassinationRogueStrategy(ai); };
                _creators["outlaw"]         = [](PsychobotAI* ai) -> Strategy* { return new OutlawRogueStrategy(ai); };
                _creators["subtlety"]       = [](PsychobotAI* ai) -> Strategy* { return new SubtletyRogueStrategy(ai); };
                _creators["rogue nc"]       = [](PsychobotAI* ai) -> Strategy* { return new GenericRogueNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterRogueStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new RogueStrategyContext());
    }
}
