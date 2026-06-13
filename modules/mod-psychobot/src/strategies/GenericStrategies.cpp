/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "GenericStrategies.h"
#include "../engine/AiObjectContext.h"

namespace psychobot
{
    // ----------------------------------------------------------------------
    // FollowStrategy (non-combat)
    // ----------------------------------------------------------------------
    NextAction** FollowStrategy::GetDefaultNonCombatActions()
    {
        return NextAction::Array(1,
            new NextAction("follow master", ACTION_IDLE),
            nullptr);
    }

    // ----------------------------------------------------------------------
    // RestStrategy (non-combat) - eat/drink + repair above plain follow.
    // ----------------------------------------------------------------------
    NextAction** RestStrategy::GetDefaultNonCombatActions()
    {
        // Rest (heal/refill) outranks follow when hurt; repair is cheap upkeep.
        return NextAction::Array(2,
            new NextAction("rest",   ACTION_NORMAL),
            new NextAction("repair", ACTION_IDLE + 1),
            nullptr);
    }

    // ----------------------------------------------------------------------
    // MeleeCombatStrategy (combat)
    // ----------------------------------------------------------------------
    NextAction** MeleeCombatStrategy::GetDefaultCombatActions()
    {
        // When nothing higher fires, keep auto-attacking the current target.
        return NextAction::Array(1,
            new NextAction("attack", ACTION_NORMAL),
            nullptr);
    }

    void MeleeCombatStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Out of melee -> close the gap (above plain auto-attack).
        triggers.push_back(new TriggerNode("enemy out of melee",
            NextAction::Array(1, new NextAction("reach melee", ACTION_MOVE), nullptr)));

        // In melee with a valid target -> auto-attack.
        triggers.push_back(new TriggerNode("enemy is close",
            NextAction::Array(1, new NextAction("attack", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // RangedCombatStrategy (combat)
    // ----------------------------------------------------------------------
    NextAction** RangedCombatStrategy::GetDefaultCombatActions()
    {
        return NextAction::Array(1,
            new NextAction("attack", ACTION_NORMAL),
            nullptr);
    }

    void RangedCombatStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Keep within spell range of the target (class casts pushed in S7+).
        triggers.push_back(new TriggerNode("enemy out of melee",
            NextAction::Array(1, new NextAction("reach spell", ACTION_MOVE), nullptr)));

        triggers.push_back(new TriggerNode("has target",
            NextAction::Array(1, new NextAction("attack", ACTION_NORMAL + 1), nullptr)));
    }

    // ----------------------------------------------------------------------
    // HealStrategy (combat - placeholder)
    // ----------------------------------------------------------------------
    void HealStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Real heal spell selection arrives in S21; for now this is a wired
        // trigger with no handler action so the framework is in place.
        triggers.push_back(new TriggerNode("party member low health",
            NextAction::Array(0, nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class GenericStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            GenericStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["follow"] = [](PsychobotAI* ai) -> Strategy* { return new FollowStrategy(ai); };
                _creators["rest"]   = [](PsychobotAI* ai) -> Strategy* { return new RestStrategy(ai); };
                _creators["melee"]  = [](PsychobotAI* ai) -> Strategy* { return new MeleeCombatStrategy(ai); };
                _creators["ranged"] = [](PsychobotAI* ai) -> Strategy* { return new RangedCombatStrategy(ai); };
                _creators["tank"]   = [](PsychobotAI* ai) -> Strategy* { return new TankCombatStrategy(ai); };
                _creators["heal"]   = [](PsychobotAI* ai) -> Strategy* { return new HealStrategy(ai); };
            }
        };
    }

    void RegisterGenericStrategies(AiObjectContext* context)
    {
        if (context)
            context->AddStrategyContext(new GenericStrategyContext());
    }
}
