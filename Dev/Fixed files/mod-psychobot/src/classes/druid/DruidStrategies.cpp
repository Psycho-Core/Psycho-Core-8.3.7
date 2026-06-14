/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Druid rotations. Relevance scale (ACTION_NORMAL = 10). FORM upkeep is
 *  the highest combat priority (a Druid does nothing useful in the wrong form),
 *  then bleeds/DoTs, then spenders, then fillers. Heals use ACTION_*_HEAL tiers.
 *  Combo-point finishers gate on "combo points full". All spells by NAME.
 * ===========================================================================
 */

#include "DruidStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const D_FILLER = ACTION_NORMAL + 1;     // 11 - fillers / builders
    static float const D_DOT    = ACTION_NORMAL + 3;     // 13 - DoT/bleed upkeep
    static float const D_SPEND  = ACTION_NORMAL + 4;     // 14 - resource spenders
    static float const D_MITI   = ACTION_NORMAL + 5;     // 15 - tank mitigation
    static float const D_FORM   = ACTION_HIGH + 5;       // 25 - shift into form
    static float const D_DEFEND = ACTION_CRITICAL_HEAL;  // 80 - Barkskin / self-heal

    // ----------------------------------------------------------------------
    // GenericDruidStrategy - shared (self-heal + defensive).
    // ----------------------------------------------------------------------
    void GenericDruidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Barkskin defensive when hurt (usable in any form).
        triggers.push_back(new TriggerNode("critical health",
            NextAction::Array(1, new NextAction("cast self::Barkskin", D_DEFEND), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Balance (Moonkin Form, ranged)
    // ----------------------------------------------------------------------
    NextAction** BalanceDruidStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast::Wrath", D_FILLER),
            new NextAction("attack",      ACTION_NORMAL),
            nullptr);
    }

    void BalanceDruidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDruidStrategy::InitCombatTriggers(triggers);

        // Shift into Moonkin Form first.
        triggers.push_back(new TriggerNode("moonkin form absent",
            NextAction::Array(1, new NextAction("cast self::Moonkin Form", D_FORM), nullptr)));

        // Keep both DoTs up.
        triggers.push_back(new TriggerNode("moonfire absent",
            NextAction::Array(1, new NextAction("cast::Moonfire", D_DOT), nullptr)));
        triggers.push_back(new TriggerNode("sunfire absent",
            NextAction::Array(1, new NextAction("cast::Sunfire", D_DOT), nullptr)));

        // Spend Astral Power on Starsurge.
        triggers.push_back(new TriggerNode("astral power high",
            NextAction::Array(1, new NextAction("cast::Starsurge", D_SPEND), nullptr)));

        // AoE: Starfall.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Starfall", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Feral (Cat Form, melee)
    // ----------------------------------------------------------------------
    NextAction** FeralDruidStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast melee::Shred", D_FILLER),
            new NextAction("attack",            ACTION_NORMAL),
            nullptr);
    }

    void FeralDruidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDruidStrategy::InitCombatTriggers(triggers);

        // Shift into Cat Form first.
        triggers.push_back(new TriggerNode("cat form absent",
            NextAction::Array(1, new NextAction("cast self::Cat Form", D_FORM), nullptr)));

        // Finisher at >=5 combo points (Rip if missing, else Ferocious Bite).
        triggers.push_back(new TriggerNode("rip absent",
            NextAction::Array(1, new NextAction("cast melee::Rip", D_DOT + 1), nullptr)));
        triggers.push_back(new TriggerNode("combo points full",
            NextAction::Array(1, new NextAction("cast melee::Ferocious Bite", D_SPEND), nullptr)));

        // Keep Rake bleed up.
        triggers.push_back(new TriggerNode("rake absent",
            NextAction::Array(1, new NextAction("cast melee::Rake", D_DOT), nullptr)));

        // Tiger's Fury burst CD.
        triggers.push_back(new TriggerNode("spell ready::Tiger's Fury",
            NextAction::Array(1, new NextAction("cast self::Tiger's Fury", D_FILLER + 1), nullptr)));

        // AoE: Swipe.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Swipe", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Guardian (Bear Form, tank)
    // ----------------------------------------------------------------------
    NextAction** GuardianDruidStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(3,
            new NextAction("cast melee::Mangle", D_FILLER + 1),
            new NextAction("cast melee::Maul",   D_FILLER),
            new NextAction("attack",             ACTION_NORMAL),
            nullptr);
    }

    void GuardianDruidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDruidStrategy::InitCombatTriggers(triggers);

        // Shift into Bear Form first.
        triggers.push_back(new TriggerNode("bear form absent",
            NextAction::Array(1, new NextAction("cast self::Bear Form", D_FORM), nullptr)));

        // Active mitigation: Ironfur; Frenzied Regeneration self-heal when hurt.
        triggers.push_back(new TriggerNode("ironfur absent",
            NextAction::Array(1, new NextAction("cast self::Ironfur", D_MITI), nullptr)));
        triggers.push_back(new TriggerNode("low health",
            NextAction::Array(1, new NextAction("cast self::Frenzied Regeneration", ACTION_MEDIUM_HEAL), nullptr)));

        // Mangle on cooldown (rage + threat).
        triggers.push_back(new TriggerNode("spell ready::Mangle",
            NextAction::Array(1, new NextAction("cast melee::Mangle", D_FILLER + 1), nullptr)));

        // Keep Thrash bleed up (AoE threat).
        triggers.push_back(new TriggerNode("thrash absent",
            NextAction::Array(1, new NextAction("cast melee aoe::Thrash", D_DOT), nullptr)));

        // Dump rage with Maul.
        triggers.push_back(new TriggerNode("rage high",
            NextAction::Array(1, new NextAction("cast melee::Maul", D_FILLER), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Restoration (caster healer)
    // ----------------------------------------------------------------------
    NextAction** RestorationDruidStrategy::GetDefaultCombatActions()
    {
        // Contribute damage when nobody needs healing (caster form, no shift).
        return NextAction::Array(2,
            new NextAction("cast::Wrath", D_FILLER),
            new NextAction("attack",      ACTION_NORMAL),
            nullptr);
    }

    void RestorationDruidStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericDruidStrategy::InitCombatTriggers(triggers);

        // HoT + spot heals on the most-injured ally.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Rejuvenation", ACTION_LIGHT_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Swiftmend", ACTION_CRITICAL_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Regrowth", ACTION_MEDIUM_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Wild Growth", ACTION_MEDIUM_HEAL - 1), nullptr)));

        // S21: cure a debuffed ally (Nature's Cure removes Magic/Curse/Poison).
        triggers.push_back(new TriggerNode("party member needs dispel",
            NextAction::Array(1, new NextAction("cast dispel::Nature's Cure", ACTION_DISPEL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: Mark of the Wild upkeep
    // ----------------------------------------------------------------------
    void GenericDruidNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        triggers.push_back(new TriggerNode("no aura::Mark of the Wild",
            NextAction::Array(1, new NextAction("cast buff::Mark of the Wild", ACTION_IDLE + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class DruidStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            DruidStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["druid"]       = [](PsychobotAI* ai) -> Strategy* { return new GenericDruidStrategy(ai); };
                _creators["balance"]     = [](PsychobotAI* ai) -> Strategy* { return new BalanceDruidStrategy(ai); };
                _creators["feral"]       = [](PsychobotAI* ai) -> Strategy* { return new FeralDruidStrategy(ai); };
                _creators["guardian"]    = [](PsychobotAI* ai) -> Strategy* { return new GuardianDruidStrategy(ai); };
                _creators["restoration"] = [](PsychobotAI* ai) -> Strategy* { return new RestorationDruidStrategy(ai); };
                _creators["druid nc"]    = [](PsychobotAI* ai) -> Strategy* { return new GenericDruidNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterDruidStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new DruidStrategyContext());
    }
}
