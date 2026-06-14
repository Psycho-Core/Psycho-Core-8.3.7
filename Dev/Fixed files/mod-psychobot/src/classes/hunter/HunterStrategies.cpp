/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Hunter rotations. Relevance scale (ACTION_NORMAL = 10). Pet upkeep
 *  (summon/revive) outranks the rotation for BM/Survival since those specs do
 *  little damage without a pet. Execute (Kill Shot) and DoT upkeep sit above
 *  fillers; Focus dumps above builders.
 *  All spells referenced by NAME (CastSpell family resolves the bot's known id).
 * ===========================================================================
 */

#include "HunterStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const H_BUILDER  = ACTION_NORMAL + 1;     // 11 - Focus builders
    static float const H_FILLER   = ACTION_NORMAL + 2;     // 12 - main fillers
    static float const H_SPEND    = ACTION_NORMAL + 3;     // 13 - Focus dumps
    static float const H_KEY      = ACTION_NORMAL + 4;     // 14 - signature
    static float const H_DOT      = ACTION_NORMAL + 5;     // 15 - DoT upkeep
    static float const H_EXECUTE  = ACTION_NORMAL + 6;     // 16 - Kill Shot
    static float const H_PET      = ACTION_HIGH + 5;       // 25 - summon/revive pet
    static float const H_EMERGENCY= ACTION_CRITICAL_HEAL;  // 80 - Exhilaration

    // ----------------------------------------------------------------------
    // GenericHunterStrategy - shared (execute + self-heal).
    // ----------------------------------------------------------------------
    void GenericHunterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Kill Shot in the execute window.
        triggers.push_back(new TriggerNode("target low health",
            NextAction::Array(1, new NextAction("cast::Kill Shot", H_EXECUTE), nullptr)));

        // Exhilaration self-heal when hurt.
        triggers.push_back(new TriggerNode("low health",
            NextAction::Array(1, new NextAction("cast self::Exhilaration", H_EMERGENCY), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Beast Mastery (ranged, pet-centric)
    // ----------------------------------------------------------------------
    NextAction** BeastMasteryHunterStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast::Kill Command", H_KEY),
            new NextAction("cast::Barbed Shot",  H_FILLER),
            new NextAction("cast::Cobra Shot",   H_SPEND),
            new NextAction("attack",             ACTION_NORMAL),
            nullptr);
    }

    void BeastMasteryHunterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericHunterStrategy::InitCombatTriggers(triggers);

        // No pet in combat -> revive/summon immediately (BM is dead weight w/o it).
        triggers.push_back(new TriggerNode("no pet",
            NextAction::Array(2,
                new NextAction("cast self::Revive Pet", H_PET),
                new NextAction("cast self::Call Pet 1", H_PET - 1),
                nullptr)));

        // Barbed Shot to keep Frenzy on the pet (and feed Focus).
        triggers.push_back(new TriggerNode("spell ready::Barbed Shot",
            NextAction::Array(1, new NextAction("cast::Barbed Shot", H_FILLER + 1), nullptr)));

        // Bestial Wrath burst CD.
        triggers.push_back(new TriggerNode("spell ready::Bestial Wrath",
            NextAction::Array(1, new NextAction("cast self::Bestial Wrath", H_KEY + 1), nullptr)));

        // Dump Focus with Cobra Shot when capped.
        triggers.push_back(new TriggerNode("focus high",
            NextAction::Array(1, new NextAction("cast::Cobra Shot", H_SPEND), nullptr)));

        // AoE: Multi-Shot to spread Beast Cleave.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Multi-Shot", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Marksmanship (ranged, no pet needed)
    // ----------------------------------------------------------------------
    NextAction** MarksmanshipHunterStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast::Aimed Shot",  H_KEY),
            new NextAction("cast::Arcane Shot", H_SPEND),
            new NextAction("cast::Steady Shot", H_BUILDER),
            new NextAction("attack",            ACTION_NORMAL),
            nullptr);
    }

    void MarksmanshipHunterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericHunterStrategy::InitCombatTriggers(triggers);

        // Keep Serpent Sting up.
        triggers.push_back(new TriggerNode("serpent sting absent",
            NextAction::Array(1, new NextAction("cast::Serpent Sting", H_DOT), nullptr)));

        // Rapid Fire on cooldown.
        triggers.push_back(new TriggerNode("spell ready::Rapid Fire",
            NextAction::Array(1, new NextAction("cast::Rapid Fire", H_KEY + 1), nullptr)));

        // Trueshot burst CD.
        triggers.push_back(new TriggerNode("spell ready::Trueshot",
            NextAction::Array(1, new NextAction("cast self::Trueshot", H_KEY + 1), nullptr)));

        // Dump Focus with Arcane Shot.
        triggers.push_back(new TriggerNode("focus high",
            NextAction::Array(1, new NextAction("cast::Arcane Shot", H_SPEND), nullptr)));

        // AoE: Multi-Shot (Trick Shots).
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Multi-Shot", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Survival (MELEE, pet-centric)
    // ----------------------------------------------------------------------
    NextAction** SurvivalHunterStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast::Kill Command",         H_KEY),
            new NextAction("cast melee::Raptor Strike",  H_SPEND),
            new NextAction("cast::Wildfire Bomb",        H_FILLER),
            new NextAction("attack",                     ACTION_NORMAL),
            nullptr);
    }

    void SurvivalHunterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericHunterStrategy::InitCombatTriggers(triggers);

        // No pet in combat -> revive/summon (Survival also needs the pet).
        triggers.push_back(new TriggerNode("no pet",
            NextAction::Array(2,
                new NextAction("cast self::Revive Pet", H_PET),
                new NextAction("cast self::Call Pet 1", H_PET - 1),
                nullptr)));

        // Wildfire Bomb on cooldown (key DoT/AoE).
        triggers.push_back(new TriggerNode("spell ready::Wildfire Bomb",
            NextAction::Array(1, new NextAction("cast::Wildfire Bomb", H_KEY), nullptr)));

        // Keep Serpent Sting up.
        triggers.push_back(new TriggerNode("serpent sting absent",
            NextAction::Array(1, new NextAction("cast::Serpent Sting", H_DOT), nullptr)));

        // Coordinated Assault burst CD.
        triggers.push_back(new TriggerNode("spell ready::Coordinated Assault",
            NextAction::Array(1, new NextAction("cast self::Coordinated Assault", H_KEY + 1), nullptr)));

        // Dump Focus with Raptor Strike (melee).
        triggers.push_back(new TriggerNode("focus high",
            NextAction::Array(1, new NextAction("cast melee::Raptor Strike", H_SPEND), nullptr)));

        // AoE: Carve.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Carve", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: pet upkeep
    // ----------------------------------------------------------------------
    void GenericHunterNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // No pet out of combat -> revive (if dead) or summon stable slot 1.
        triggers.push_back(new TriggerNode("no pet",
            NextAction::Array(2,
                new NextAction("cast self::Revive Pet", ACTION_IDLE + 2),
                new NextAction("cast self::Call Pet 1", ACTION_IDLE + 1),
                nullptr)));

        // Heal a hurt pet with Mend Pet.
        triggers.push_back(new TriggerNode("pet hurt",
            NextAction::Array(1, new NextAction("cast self::Mend Pet", ACTION_IDLE + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class HunterStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            HunterStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["hunter"]        = [](PsychobotAI* ai) -> Strategy* { return new GenericHunterStrategy(ai); };
                _creators["beast mastery"] = [](PsychobotAI* ai) -> Strategy* { return new BeastMasteryHunterStrategy(ai); };
                _creators["marksmanship"]  = [](PsychobotAI* ai) -> Strategy* { return new MarksmanshipHunterStrategy(ai); };
                _creators["survival"]      = [](PsychobotAI* ai) -> Strategy* { return new SurvivalHunterStrategy(ai); };
                _creators["hunter nc"]     = [](PsychobotAI* ai) -> Strategy* { return new GenericHunterNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterHunterStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new HunterStrategyContext());
    }
}
