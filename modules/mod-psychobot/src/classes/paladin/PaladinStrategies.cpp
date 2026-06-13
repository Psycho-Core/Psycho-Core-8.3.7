/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Paladin rotations. Relevance scale (ACTION_NORMAL = 10). Heals use
 *  the dedicated ACTION_*_HEAL tiers so a hurt party member outranks any damage.
 *  All spells referenced by NAME (CastSpell family resolves the bot's known id).
 * ===========================================================================
 */

#include "PaladinStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const P_FILLER   = ACTION_NORMAL + 1;     // 11 - HP generators
    static float const P_SPEND    = ACTION_NORMAL + 3;     // 13 - HP spenders
    static float const P_EXECUTE  = ACTION_NORMAL + 5;     // 15 - Hammer of Wrath
    static float const P_MITIGATE = ACTION_NORMAL + 6;     // 16 - SotR mitigation
    static float const P_EMERGENCY= ACTION_EMERGENCY;      // 90 - Lay on Hands

    // ----------------------------------------------------------------------
    // GenericPaladinStrategy - shared (execute + emergency self-save).
    // ----------------------------------------------------------------------
    void GenericPaladinStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Hammer of Wrath when the target enters the execute window.
        triggers.push_back(new TriggerNode("target low health",
            NextAction::Array(1, new NextAction("cast::Hammer of Wrath", P_EXECUTE), nullptr)));

        // Emergency self-save when critically low.
        triggers.push_back(new TriggerNode("critical health",
            NextAction::Array(2,
                new NextAction("cast self::Lay on Hands", P_EMERGENCY),
                new NextAction("cast self::Divine Shield", P_EMERGENCY - 1),
                nullptr)));
    }

    // ----------------------------------------------------------------------
    // Holy (healer)
    // ----------------------------------------------------------------------
    NextAction** HolyPaladinStrategy::GetDefaultCombatActions()
    {
        // When nobody needs healing, contribute damage (and generate Holy Power).
        return NextAction::Array(2,
            new NextAction("cast::Judgment",        P_FILLER),
            new NextAction("cast melee::Crusader Strike", P_FILLER - 1),
            nullptr);
    }

    void HolyPaladinStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericPaladinStrategy::InitCombatTriggers(triggers);

        // Someone hurt -> Holy Shock (key heal, fast), then Word of Glory if we
        // have Holy Power, then Flash of Light as a top-up.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Holy Shock", ACTION_MEDIUM_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("holy power high",
            NextAction::Array(1, new NextAction("cast heal::Word of Glory", ACTION_LIGHT_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Flash of Light", ACTION_LIGHT_HEAL - 1), nullptr)));

        // S21: cure a debuffed ally (Cleanse removes Magic/Poison/Disease).
        triggers.push_back(new TriggerNode("party member needs dispel",
            NextAction::Array(1, new NextAction("cast dispel::Cleanse", ACTION_DISPEL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Protection (tank)
    // ----------------------------------------------------------------------
    NextAction** ProtectionPaladinStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast::Avenger's Shield",        P_FILLER + 2),
            new NextAction("cast::Judgment",                P_FILLER + 1),
            new NextAction("cast melee::Hammer of the Righteous", P_FILLER),
            new NextAction("attack",                        ACTION_NORMAL),
            nullptr);
    }

    void ProtectionPaladinStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericPaladinStrategy::InitCombatTriggers(triggers);

        // Avenger's Shield on cooldown (ranged pull + threat).
        triggers.push_back(new TriggerNode("spell ready::Avenger's Shield",
            NextAction::Array(1, new NextAction("cast::Avenger's Shield", P_FILLER + 2), nullptr)));

        // Spend Holy Power on Shield of the Righteous (mitigation).
        triggers.push_back(new TriggerNode("holy power high",
            NextAction::Array(1, new NextAction("cast self::Shield of the Righteous", P_MITIGATE), nullptr)));

        // AoE: Consecration under the pack.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast self::Consecration", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Retribution (dps)
    // ----------------------------------------------------------------------
    NextAction** RetributionPaladinStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast::Judgment",         P_FILLER + 1),
            new NextAction("cast melee::Blade of Justice", P_FILLER + 2),
            new NextAction("cast melee::Crusader Strike",  P_FILLER),
            new NextAction("attack",                 ACTION_NORMAL),
            nullptr);
    }

    void RetributionPaladinStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericPaladinStrategy::InitCombatTriggers(triggers);

        // Spend 3 Holy Power on Templar's Verdict (single) / Divine Storm (AoE).
        triggers.push_back(new TriggerNode("holy power high",
            NextAction::Array(1, new NextAction("cast melee::Templar's Verdict", P_SPEND), nullptr)));
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Divine Storm", ACTION_NORMAL + 1), nullptr)));

        // Avenging Wrath burst CD.
        triggers.push_back(new TriggerNode("spell ready::Avenging Wrath",
            NextAction::Array(1, new NextAction("cast self::Avenging Wrath", P_FILLER + 3), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: aura + blessing upkeep
    // ----------------------------------------------------------------------
    void GenericPaladinNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        triggers.push_back(new TriggerNode("devotion aura absent",
            NextAction::Array(1, new NextAction("cast self::Devotion Aura", ACTION_IDLE + 1), nullptr)));
        // Blessing of Kings if known and missing (cast buff = self, aura-gated).
        triggers.push_back(new TriggerNode("no aura::Blessing of Kings",
            NextAction::Array(1, new NextAction("cast buff::Blessing of Kings", ACTION_IDLE), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class PaladinStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            PaladinStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["paladin"]     = [](PsychobotAI* ai) -> Strategy* { return new GenericPaladinStrategy(ai); };
                _creators["holy"]        = [](PsychobotAI* ai) -> Strategy* { return new HolyPaladinStrategy(ai); };
                _creators["protection"]  = [](PsychobotAI* ai) -> Strategy* { return new ProtectionPaladinStrategy(ai); };
                _creators["retribution"] = [](PsychobotAI* ai) -> Strategy* { return new RetributionPaladinStrategy(ai); };
                _creators["paladin nc"]  = [](PsychobotAI* ai) -> Strategy* { return new GenericPaladinNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterPaladinStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new PaladinStrategyContext());
    }
}
