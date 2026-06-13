/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  BfA 8.3 Mage rotations. Relevance scale (ACTION_NORMAL = 10). PROC reactions
 *  (Clearcasting / Hot Streak / Brain Freeze / Fingers of Frost) sit above the
 *  fillers so they're consumed immediately; resource dumps (Arcane Barrage at
 *  high Charges) above builders. All spells referenced by NAME.
 * ===========================================================================
 */

#include "MageStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    static float const M_FILLER = ACTION_NORMAL + 1;     // 11 - main filler
    static float const M_BUILD  = ACTION_NORMAL + 2;     // 12 - charge builder
    static float const M_SPEND  = ACTION_NORMAL + 4;     // 14 - dump charges
    static float const M_PROC   = ACTION_NORMAL + 5;     // 15 - proc reaction
    static float const M_CD     = ACTION_NORMAL + 3;     // 13 - offensive CDs
    static float const M_SAVE   = ACTION_EMERGENCY;      // 90 - Ice Block

    // ----------------------------------------------------------------------
    // GenericMageStrategy - shared (interrupt + self-save + Frost Nova).
    // ----------------------------------------------------------------------
    void GenericMageStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Counterspell interrupt on a casting target.
        triggers.push_back(new TriggerNode("target casting",
            NextAction::Array(1, new NextAction("cast::Counterspell", ACTION_INTERRUPT), nullptr)));

        // Ice Block when critically low.
        triggers.push_back(new TriggerNode("critical health",
            NextAction::Array(1, new NextAction("cast self::Ice Block", M_SAVE), nullptr)));

        // Frost Nova to root an enemy that reached melee, then we kite.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast self::Frost Nova", ACTION_MOVE), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Arcane
    // ----------------------------------------------------------------------
    NextAction** ArcaneMageStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast::Arcane Blast", M_BUILD),
            new NextAction("attack",             ACTION_NORMAL),
            nullptr);
    }

    void ArcaneMageStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericMageStrategy::InitCombatTriggers(triggers);

        // Clearcasting -> free Arcane Missiles.
        triggers.push_back(new TriggerNode("clearcasting",
            NextAction::Array(1, new NextAction("cast::Arcane Missiles", M_PROC), nullptr)));

        // Dump charges with Arcane Barrage when high.
        triggers.push_back(new TriggerNode("arcane charges high",
            NextAction::Array(1, new NextAction("cast::Arcane Barrage", M_SPEND), nullptr)));

        // Arcane Power burst CD.
        triggers.push_back(new TriggerNode("spell ready::Arcane Power",
            NextAction::Array(1, new NextAction("cast self::Arcane Power", M_CD), nullptr)));

        // AoE: Arcane Explosion.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast self::Arcane Explosion", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Fire
    // ----------------------------------------------------------------------
    NextAction** FireMageStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast::Fireball", M_FILLER),
            new NextAction("attack",         ACTION_NORMAL),
            nullptr);
    }

    void FireMageStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericMageStrategy::InitCombatTriggers(triggers);

        // Hot Streak -> instant Pyroblast.
        triggers.push_back(new TriggerNode("hot streak",
            NextAction::Array(1, new NextAction("cast::Pyroblast", M_PROC), nullptr)));

        // Fire Blast on cooldown (banks crit toward Hot Streak).
        triggers.push_back(new TriggerNode("spell ready::Fire Blast",
            NextAction::Array(1, new NextAction("cast::Fire Blast", M_BUILD), nullptr)));

        // Combustion burst CD.
        triggers.push_back(new TriggerNode("spell ready::Combustion",
            NextAction::Array(1, new NextAction("cast self::Combustion", M_CD), nullptr)));

        // Execute / on the move: Scorch.
        triggers.push_back(new TriggerNode("target low health",
            NextAction::Array(1, new NextAction("cast::Scorch", M_FILLER + 1), nullptr)));

        // AoE: Flamestrike.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Flamestrike", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Frost
    // ----------------------------------------------------------------------
    NextAction** FrostMageStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(2,
            new NextAction("cast::Frostbolt", M_FILLER),
            new NextAction("attack",          ACTION_NORMAL),
            nullptr);
    }

    void FrostMageStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        GenericMageStrategy::InitCombatTriggers(triggers);

        // Brain Freeze -> instant Flurry.
        triggers.push_back(new TriggerNode("brain freeze",
            NextAction::Array(1, new NextAction("cast::Flurry", M_PROC), nullptr)));

        // Fingers of Frost -> shatter with Ice Lance.
        triggers.push_back(new TriggerNode("fingers of frost",
            NextAction::Array(1, new NextAction("cast::Ice Lance", M_PROC), nullptr)));

        // Frozen Orb on cooldown.
        triggers.push_back(new TriggerNode("spell ready::Frozen Orb",
            NextAction::Array(1, new NextAction("cast::Frozen Orb", M_CD), nullptr)));

        // Icy Veins burst CD.
        triggers.push_back(new TriggerNode("spell ready::Icy Veins",
            NextAction::Array(1, new NextAction("cast self::Icy Veins", M_CD), nullptr)));

        // AoE: Blizzard.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("cast::Blizzard", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Non-combat: Arcane Intellect + Conjure Refreshment
    // ----------------------------------------------------------------------
    void GenericMageNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        triggers.push_back(new TriggerNode("arcane intellect absent",
            NextAction::Array(1, new NextAction("cast buff::Arcane Intellect", ACTION_IDLE + 1), nullptr)));

        // Conjure food/water when off cooldown (mana/health upkeep).
        triggers.push_back(new TriggerNode("spell ready::Conjure Refreshment",
            NextAction::Array(1, new NextAction("cast self::Conjure Refreshment", ACTION_IDLE), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class MageStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            MageStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["mage"]    = [](PsychobotAI* ai) -> Strategy* { return new GenericMageStrategy(ai); };
                _creators["arcane"]  = [](PsychobotAI* ai) -> Strategy* { return new ArcaneMageStrategy(ai); };
                _creators["fire"]    = [](PsychobotAI* ai) -> Strategy* { return new FireMageStrategy(ai); };
                _creators["frost"]   = [](PsychobotAI* ai) -> Strategy* { return new FrostMageStrategy(ai); };
                _creators["mage nc"] = [](PsychobotAI* ai) -> Strategy* { return new GenericMageNonCombatStrategy(ai); };
            }
        };
    }

    void RegisterMageStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new MageStrategyContext());
    }
}
