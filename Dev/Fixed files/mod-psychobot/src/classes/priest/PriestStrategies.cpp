/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Priest rotations. Relevance scale (ACTION_NORMAL = 10). Heals use the
 *  ACTION_*_HEAL tiers so a hurt ally outranks any damage. Discipline keeps
 *  Atonement up then deals damage (which heals atonement'd allies); Holy uses
 *  direct heals; Shadow maintains DoTs + spends Insanity.
 *  All spells referenced by NAME (CastSpell family resolves the bot's known id).
 * ===========================================================================
 */

#include "PriestStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const PR_FILLER  = ACTION_NORMAL + 1;     // 11 - damage fillers
    static float const PR_DOT     = ACTION_NORMAL + 3;     // 13 - DoT upkeep
    static float const PR_SPEND   = ACTION_NORMAL + 4;     // 14 - Insanity spend
    static float const PR_EXECUTE = ACTION_NORMAL + 5;     // 15 - SW:Death execute
    static float const PR_ATONE   = ACTION_LIGHT_HEAL + 5; // 65 - keep Atonement up
    static float const PR_EMERG   = ACTION_EMERGENCY;      // 90 - Desperate Prayer

    // ----------------------------------------------------------------------
    // GenericPriestStrategy - shared (shield ally, self-save, execute).
    // ----------------------------------------------------------------------
    void GenericPriestStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Shield a hurt party member (also a Disc atonement applicator).
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Power Word: Shield", ACTION_LIGHT_HEAL), nullptr)));

        // Desperate Prayer self-save when critical.
        triggers.push_back(new TriggerNode("critical health",
            NextAction::Array(1, new NextAction("cast self::Desperate Prayer", PR_EMERG), nullptr)));

        // Shadow Word: Death execute.
        triggers.push_back(new TriggerNode("target low health",
            NextAction::Array(1, new NextAction("cast::Shadow Word: Death", PR_EXECUTE), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Discipline (atonement healer)
    // ----------------------------------------------------------------------
    NextAction** DisciplinePriestStrategy::GetDefaultCombatActions()
    {
        // Damage converts to healing via Atonement; Smite is the filler.
        return NextAction::Array(2,
            new NextAction("cast::Smite", PR_FILLER),
            new NextAction("attack",      ACTION_NORMAL),
            nullptr);
    }

    void DisciplinePriestStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericPriestStrategy::InitCombatTriggers(triggers);

        // Apply/refresh Atonement on a hurt ally (PWS), AoE via Radiance.
        triggers.push_back(new TriggerNode("atonement low",
            NextAction::Array(1, new NextAction("cast heal::Power Word: Shield", PR_ATONE), nullptr)));

        // Penance: direct heal on a hurt ally / damage otherwise.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Penance", ACTION_MEDIUM_HEAL), nullptr)));

        // Keep Shadow Word: Pain ticking (atonement damage source).
        triggers.push_back(new TriggerNode("shadow word pain absent",
            NextAction::Array(1, new NextAction("cast::Shadow Word: Pain", PR_DOT), nullptr)));

        // Pain Suppression on a critically low ally.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Pain Suppression", ACTION_CRITICAL_HEAL), nullptr)));

        // S21: cure a debuffed ally (Purify removes Magic + Disease).
        triggers.push_back(new TriggerNode("party member needs dispel",
            NextAction::Array(1, new NextAction("cast dispel::Purify", ACTION_DISPEL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Holy (direct healer)
    // ----------------------------------------------------------------------
    NextAction** HolyPriestStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast::Smite", PR_FILLER),
            new NextAction("attack",      ACTION_NORMAL),
            nullptr);
    }

    void HolyPriestStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericPriestStrategy::InitCombatTriggers(triggers);

        // Tiered heals on the most-injured ally.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Holy Word: Serenity", ACTION_CRITICAL_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Flash Heal", ACTION_MEDIUM_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Heal", ACTION_LIGHT_HEAL), nullptr)));

        // Guardian Spirit on a critically low ally.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Guardian Spirit", ACTION_EMERGENCY), nullptr)));

        // S21: cure a debuffed ally (Purify removes Magic + Disease).
        triggers.push_back(new TriggerNode("party member needs dispel",
            NextAction::Array(1, new NextAction("cast dispel::Purify", ACTION_DISPEL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Shadow (dps)
    // ----------------------------------------------------------------------
    NextAction** ShadowPriestStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(3,
            new NextAction("cast::Mind Blast", PR_FILLER + 1),
            new NextAction("cast::Mind Flay",  PR_FILLER),
            new NextAction("attack",           ACTION_NORMAL),
            nullptr);
    }

    void ShadowPriestStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericPriestStrategy::InitCombatTriggers(triggers);

        // Keep both DoTs up.
        triggers.push_back(new TriggerNode("shadow word pain absent",
            NextAction::Array(1, new NextAction("cast::Shadow Word: Pain", PR_DOT), nullptr)));
        triggers.push_back(new TriggerNode("vampiric touch absent",
            NextAction::Array(1, new NextAction("cast::Vampiric Touch", PR_DOT), nullptr)));

        // Spend Insanity on Devouring Plague (or Void Eruption to enter Voidform).
        triggers.push_back(new TriggerNode("insanity high",
            NextAction::Array(2,
                new NextAction("cast::Void Eruption", PR_SPEND + 1),
                new NextAction("cast::Devouring Plague", PR_SPEND),
                nullptr)));

        // Shadowfiend burst CD.
        triggers.push_back(new TriggerNode("spell ready::Shadowfiend",
            NextAction::Array(1, new NextAction("cast::Shadowfiend", PR_FILLER + 2), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: Shadowform / Fortitude upkeep
    // ----------------------------------------------------------------------
    void GenericPriestNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Shadow: stay in Shadowform.
        triggers.push_back(new TriggerNode("shadowform absent",
            NextAction::Array(1, new NextAction("cast self::Shadowform", ACTION_IDLE + 1), nullptr)));

        // Power Word: Fortitude buff if known and missing.
        triggers.push_back(new TriggerNode("no aura::Power Word: Fortitude",
            NextAction::Array(1, new NextAction("cast buff::Power Word: Fortitude", ACTION_IDLE), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class PriestStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            PriestStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["priest"]     = [](PsychobotAI* ai) -> Strategy* { return new GenericPriestStrategy(ai); };
                _creators["discipline"] = [](PsychobotAI* ai) -> Strategy* { return new DisciplinePriestStrategy(ai); };
                _creators["holy"]       = [](PsychobotAI* ai) -> Strategy* { return new HolyPriestStrategy(ai); };
                _creators["shadow"]     = [](PsychobotAI* ai) -> Strategy* { return new ShadowPriestStrategy(ai); };
                _creators["priest nc"]  = [](PsychobotAI* ai) -> Strategy* { return new GenericPriestNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterPriestStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new PriestStrategyContext());
    }
}
