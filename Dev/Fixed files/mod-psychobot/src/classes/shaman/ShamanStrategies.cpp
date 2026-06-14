/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Shaman rotations. Relevance scale (ACTION_NORMAL = 10). Heals use the
 *  ACTION_*_HEAL tiers. Maelstrom (Elemental) spent via Earth Shock when high;
 *  Maelstrom Weapon (Enhancement) consumed by instant Lightning Bolt at stacks.
 *  Weapon imbues + Lightning Shield are non-combat upkeep. All spells by NAME.
 * ===========================================================================
 */

#include "ShamanStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const S_FILLER  = ACTION_NORMAL + 1;     // 11 - fillers
    static float const S_DOT     = ACTION_NORMAL + 3;     // 13 - Flame Shock
    static float const S_KEY     = ACTION_NORMAL + 4;     // 14 - signature
    static float const S_SPEND   = ACTION_NORMAL + 5;     // 15 - maelstrom spend
    static float const S_CD      = ACTION_NORMAL + 2;     // 12 - offensive CDs
    static float const S_EMERG   = ACTION_CRITICAL_HEAL;  // 80 - self Healing Surge

    // ----------------------------------------------------------------------
    // GenericShamanStrategy - shared (interrupt + self-heal).
    // ----------------------------------------------------------------------
    void GenericShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Wind Shear interrupt on a casting target.
        triggers.push_back(new TriggerNode("target casting",
            NextAction::Array(1, new NextAction("cast::Wind Shear", ACTION_INTERRUPT), nullptr)));

        // Healing Surge self-heal when hurt.
        triggers.push_back(new TriggerNode("low health",
            NextAction::Array(1, new NextAction("cast self::Healing Surge", S_EMERG), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Elemental (ranged dps)
    // ----------------------------------------------------------------------
    NextAction** ElementalShamanStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast::Lava Burst",     S_KEY),
            new NextAction("cast::Earth Shock",    S_SPEND),
            new NextAction("cast::Lightning Bolt", S_FILLER),
            new NextAction("attack",               ACTION_NORMAL),
            nullptr);
    }

    void ElementalShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericShamanStrategy::InitCombatTriggers(triggers);

        // Keep Flame Shock up (enables Lava Burst auto-crit).
        triggers.push_back(new TriggerNode("flame shock absent",
            NextAction::Array(1, new NextAction("cast::Flame Shock", S_DOT), nullptr)));

        // Lava Burst on cooldown.
        triggers.push_back(new TriggerNode("spell ready::Lava Burst",
            NextAction::Array(1, new NextAction("cast::Lava Burst", S_KEY), nullptr)));

        // Spend Maelstrom on Earth Shock when high.
        triggers.push_back(new TriggerNode("maelstrom high",
            NextAction::Array(1, new NextAction("cast::Earth Shock", S_SPEND), nullptr)));

        // Fire Elemental burst CD.
        triggers.push_back(new TriggerNode("spell ready::Fire Elemental",
            NextAction::Array(1, new NextAction("cast self::Fire Elemental", S_CD), nullptr)));

        // AoE: Chain Lightning / Earthquake.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Chain Lightning", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Enhancement (melee dps)
    // ----------------------------------------------------------------------
    NextAction** EnhancementShamanStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(4,
            new NextAction("cast melee::Stormstrike", S_KEY),
            new NextAction("cast melee::Lava Lash",   S_FILLER + 1),
            new NextAction("cast::Lightning Bolt",    S_FILLER),
            new NextAction("attack",                  ACTION_NORMAL),
            nullptr);
    }

    void EnhancementShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericShamanStrategy::InitCombatTriggers(triggers);

        // Spend Maelstrom Weapon stacks on an instant Lightning Bolt.
        triggers.push_back(new TriggerNode("maelstrom weapon high",
            NextAction::Array(1, new NextAction("cast::Lightning Bolt", S_SPEND), nullptr)));

        // Stormstrike on cooldown (signature).
        triggers.push_back(new TriggerNode("spell ready::Stormstrike",
            NextAction::Array(1, new NextAction("cast melee::Stormstrike", S_KEY), nullptr)));

        // Keep Flame Shock up.
        triggers.push_back(new TriggerNode("flame shock absent",
            NextAction::Array(1, new NextAction("cast::Flame Shock", S_DOT), nullptr)));

        // Feral Spirit burst CD.
        triggers.push_back(new TriggerNode("spell ready::Feral Spirit",
            NextAction::Array(1, new NextAction("cast self::Feral Spirit", S_CD), nullptr)));

        // AoE: Crash Lightning.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast melee aoe::Crash Lightning", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Restoration (healer)
    // ----------------------------------------------------------------------
    NextAction** RestorationShamanStrategy::GetDefaultCombatActions()
    {
        // Contribute damage when nobody needs healing.
        return NextAction::Array(2,
            new NextAction("cast::Lightning Bolt", S_FILLER),
            new NextAction("attack",               ACTION_NORMAL),
            nullptr);
    }

    void RestorationShamanStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericShamanStrategy::InitCombatTriggers(triggers);

        // Riptide (HoT, fast) -> Healing Wave/Surge tiers on the hurt ally.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Riptide", ACTION_LIGHT_HEAL + 1), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Healing Surge", ACTION_MEDIUM_HEAL), nullptr)));
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Healing Wave", ACTION_LIGHT_HEAL), nullptr)));

        // Chain Heal for group damage (still keyed on a hurt member).
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(1, new NextAction("cast heal::Chain Heal", ACTION_MEDIUM_HEAL - 1), nullptr)));

        // S21: cure a debuffed ally (Cleanse Spirit removes Curse + Magic).
        triggers.push_back(new TriggerNode("party member needs dispel",
            NextAction::Array(1, new NextAction("cast dispel::Cleanse Spirit", ACTION_DISPEL), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: weapon imbues + Lightning Shield
    // ----------------------------------------------------------------------
    void GenericShamanNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Weapon imbues (Enhancement). Each only fires if known + missing.
        triggers.push_back(new TriggerNode("windfury weapon absent",
            NextAction::Array(1, new NextAction("cast self::Windfury Weapon", ACTION_IDLE + 1), nullptr)));
        triggers.push_back(new TriggerNode("flametongue weapon absent",
            NextAction::Array(1, new NextAction("cast self::Flametongue Weapon", ACTION_IDLE + 1), nullptr)));

        // Lightning Shield (Elemental/Enhancement).
        triggers.push_back(new TriggerNode("lightning shield absent",
            NextAction::Array(1, new NextAction("cast self::Lightning Shield", ACTION_IDLE), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class ShamanStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            ShamanStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["shaman"]      = [](PsychobotAI* ai) -> Strategy* { return new GenericShamanStrategy(ai); };
                _creators["elemental"]   = [](PsychobotAI* ai) -> Strategy* { return new ElementalShamanStrategy(ai); };
                _creators["enhancement"] = [](PsychobotAI* ai) -> Strategy* { return new EnhancementShamanStrategy(ai); };
                _creators["restoration"] = [](PsychobotAI* ai) -> Strategy* { return new RestorationShamanStrategy(ai); };
                _creators["shaman nc"]   = [](PsychobotAI* ai) -> Strategy* { return new GenericShamanNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterShamanStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new ShamanStrategyContext());
    }
}
