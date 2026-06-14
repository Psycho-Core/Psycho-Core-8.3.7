/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Death Knight rotations. Priorities use the engine's relevance scale:
 *  higher relevance is chosen first each tick. Maintenance (diseases, Bone
 *  Shield) sits above fillers; proc reactions (Killing Machine / Rime / Sudden
 *  Doom) and resource dumps (Runic Power) sit above the rune builders/spenders.
 *  All spells are referenced by NAME (the CastSpell family resolves the id the
 *  bot actually knows), so the same wiring works regardless of talents/DB2.
 * ===========================================================================
 */

#include "DKStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    // Relevance helpers (relative to ACTION_NORMAL = 10).
    static float const REL_FILLER     = ACTION_NORMAL + 1;   // 11 - rune spenders
    static float const REL_SPEND_RP   = ACTION_NORMAL + 2;   // 12 - dump runic power
    static float const REL_PROC       = ACTION_NORMAL + 4;   // 14 - proc reactions
    static float const REL_DISEASE    = ACTION_NORMAL + 5;   // 15 - keep diseases up
    static float const REL_MAINTAIN   = ACTION_NORMAL + 6;   // 16 - Bone Shield / self
    static float const REL_EMERGENCY  = ACTION_CRITICAL_HEAL; // 80 - Death Strike heal

    // ----------------------------------------------------------------------
    // GenericDKStrategy - shared DK combat (self-heal + AoE base).
    // ----------------------------------------------------------------------
    void GenericDKStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Emergency self-heal: Death Strike when hurt (all specs have it).
        triggers.push_back(new TriggerNode("low health",
            NextAction::Array(1, new NextAction("cast melee::Death Strike", REL_EMERGENCY), nullptr)));

        // Pull / gap-closer: Death Grip when the target is out of melee.
        triggers.push_back(new TriggerNode("enemy out of melee",
            NextAction::Array(1, new NextAction("cast::Death Grip", ACTION_MOVE + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Blood (tank)
    // ----------------------------------------------------------------------
    NextAction** BloodDKStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast melee::Heart Strike", REL_FILLER),
            new NextAction("cast melee::Death Strike", REL_SPEND_RP),
            new NextAction("cast melee::Marrowrend",   REL_FILLER + 1),
            new NextAction("attack",                   ACTION_NORMAL),
            nullptr);
    }

    void BloodDKStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDKStrategy::InitCombatTriggers(triggers);

        // Keep Bone Shield up (Marrowrend grants charges).
        triggers.push_back(new TriggerNode("bone shield low",
            NextAction::Array(1, new NextAction("cast melee::Marrowrend", REL_MAINTAIN), nullptr)));

        // Keep Blood Plague on the target (Blood Boil).
        triggers.push_back(new TriggerNode("blood plague absent",
            NextAction::Array(1, new NextAction("cast::Blood Boil", REL_DISEASE), nullptr)));

        // Dump Runic Power into Death Strike (heal + mitigation).
        triggers.push_back(new TriggerNode("runic power high",
            NextAction::Array(1, new NextAction("cast melee::Death Strike", REL_SPEND_RP), nullptr)));

        // AoE: Death and Decay when more than one enemy is engaged.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Death and Decay", ACTION_NORMAL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Frost (dps)
    // ----------------------------------------------------------------------
    NextAction** FrostDKStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast melee::Obliterate",  REL_FILLER),
            new NextAction("cast::Frost Strike",      REL_SPEND_RP),
            new NextAction("cast::Howling Blast",     REL_FILLER + 1),
            new NextAction("attack",                  ACTION_NORMAL),
            nullptr);
    }

    void FrostDKStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDKStrategy::InitCombatTriggers(triggers);

        // Apply / refresh Frost Fever via Howling Blast.
        triggers.push_back(new TriggerNode("frost fever absent",
            NextAction::Array(1, new NextAction("cast::Howling Blast", REL_DISEASE), nullptr)));

        // Rime proc -> free Howling Blast.
        triggers.push_back(new TriggerNode("rime",
            NextAction::Array(1, new NextAction("cast::Howling Blast", REL_PROC), nullptr)));

        // Killing Machine proc -> guaranteed-crit Obliterate.
        triggers.push_back(new TriggerNode("killing machine",
            NextAction::Array(1, new NextAction("cast melee::Obliterate", REL_PROC), nullptr)));

        // Pillar of Frost when ready (burst CD).
        triggers.push_back(new TriggerNode("spell ready::Pillar of Frost",
            NextAction::Array(1, new NextAction("cast::Pillar of Frost", REL_MAINTAIN), nullptr)));

        // Dump Runic Power with Frost Strike.
        triggers.push_back(new TriggerNode("runic power high",
            NextAction::Array(1, new NextAction("cast::Frost Strike", REL_SPEND_RP), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Unholy (dps)
    // ----------------------------------------------------------------------
    NextAction** UnholyDKStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast melee::Festering Strike", REL_FILLER),
            new NextAction("cast melee::Scourge Strike",   REL_FILLER + 1),
            new NextAction("cast::Death Coil",             REL_SPEND_RP),
            new NextAction("attack",                       ACTION_NORMAL),
            nullptr);
    }

    void UnholyDKStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDKStrategy::InitCombatTriggers(triggers);

        // Apply Virulent Plague via Outbreak.
        triggers.push_back(new TriggerNode("virulent plague absent",
            NextAction::Array(1, new NextAction("cast::Outbreak", REL_DISEASE), nullptr)));

        // Sudden Doom proc -> free Death Coil.
        triggers.push_back(new TriggerNode("sudden doom",
            NextAction::Array(1, new NextAction("cast::Death Coil", REL_PROC), nullptr)));

        // Apocalypse when ready (bursts wounds, summons ghouls).
        triggers.push_back(new TriggerNode("spell ready::Apocalypse",
            NextAction::Array(1, new NextAction("cast melee::Apocalypse", REL_PROC), nullptr)));

        // Dump Runic Power with Death Coil.
        triggers.push_back(new TriggerNode("runic power high",
            NextAction::Array(1, new NextAction("cast::Death Coil", REL_SPEND_RP), nullptr)));

        // AoE: Death and Decay.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Death and Decay", ACTION_NORMAL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat upkeep
    // ----------------------------------------------------------------------
    void GenericDKNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Summon the ghoul (Unholy) if we know Raise Dead and it's off cooldown.
        triggers.push_back(new TriggerNode("spell ready::Raise Dead",
            NextAction::Array(1, new NextAction("cast self::Raise Dead", ACTION_IDLE + 1), nullptr)));

        // Horn of Winter for the Runic Power / stats buff when off cooldown.
        triggers.push_back(new TriggerNode("spell ready::Horn of Winter",
            NextAction::Array(1, new NextAction("cast self::Horn of Winter", ACTION_IDLE + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class DKStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            DKStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["dk"]     = [](PsychobotAI* ai) -> Strategy* { return new GenericDKStrategy(ai); };
                _creators["blood"]  = [](PsychobotAI* ai) -> Strategy* { return new BloodDKStrategy(ai); };
                _creators["frost"]  = [](PsychobotAI* ai) -> Strategy* { return new FrostDKStrategy(ai); };
                _creators["unholy"] = [](PsychobotAI* ai) -> Strategy* { return new UnholyDKStrategy(ai); };
                _creators["dk nc"]  = [](PsychobotAI* ai) -> Strategy* { return new GenericDKNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterDeathKnightStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new DKStrategyContext());
    }
}
