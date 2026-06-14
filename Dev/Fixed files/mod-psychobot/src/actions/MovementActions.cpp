/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "MovementActions.h"
#include "../engine/AiObjectContext.h"
#include "../engine/ServerFacade.h"
#include "../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"

namespace psychobot
{
    // ----------------------------------------------------------------------
    // ReachTargetAction
    // ----------------------------------------------------------------------
    Unit* ReachTargetAction::GetTarget()
    {
        return PsychobotAIBridge::GetCurrentTarget(GetAI());
    }

    bool ReachTargetAction::IsUseful()
    {
        Unit* target = GetTarget();
        if (!ServerFacade::IsValidAttackTarget(GetBot(), target))
            return false;
        // Only useful while we are still outside the desired distance.
        return !ServerFacade::IsInRange(GetBot(), target, _distance);
    }

    bool ReachTargetAction::Execute(Event const& /*event*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return false;
        return PsychobotAIBridge::ReachTarget(GetAI(), target, _distance);
    }

    // ----------------------------------------------------------------------
    // FollowMasterAction
    // ----------------------------------------------------------------------
    bool FollowMasterAction::IsUseful()
    {
        return GetMaster() != nullptr;
    }

    bool FollowMasterAction::Execute(Event const& /*event*/)
    {
        return PsychobotAIBridge::FollowMaster(GetAI());
    }

    // ----------------------------------------------------------------------
    // StopMovingAction
    // ----------------------------------------------------------------------
    bool StopMovingAction::Execute(Event const& /*event*/)
    {
        return PsychobotAIBridge::StopMoving(GetAI());
    }

    // ----------------------------------------------------------------------
    // AttackAction
    // ----------------------------------------------------------------------
    Unit* AttackAction::GetTarget()
    {
        return PsychobotAIBridge::GetCurrentTarget(GetAI());
    }

    bool AttackAction::IsUseful()
    {
        Unit* target = GetTarget();
        return ServerFacade::IsValidAttackTarget(GetBot(), target);
    }

    bool AttackAction::Execute(Event const& /*event*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return false;
        return PsychobotAIBridge::AttackTarget(GetAI(), target);
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class MovementActionContext : public NamedObjectContext<Action>
        {
        public:
            MovementActionContext() : NamedObjectContext<Action>(/*shared*/ false)
            {
                _creators["reach melee"]   = [](PsychobotAI* ai) -> Action* { return new ReachTargetAction(ai, "reach melee", 4.0f); };
                _creators["reach spell"]   = [](PsychobotAI* ai) -> Action* { return new ReachTargetAction(ai, "reach spell", 25.0f); };
                _creators["follow master"] = [](PsychobotAI* ai) -> Action* { return new FollowMasterAction(ai); };
                _creators["stop"]          = [](PsychobotAI* ai) -> Action* { return new StopMovingAction(ai); };
                _creators["attack"]        = [](PsychobotAI* ai) -> Action* { return new AttackAction(ai); };
            }
        };
    }

    void RegisterMovementActions(AiObjectContext* context)
    {
        if (context)
            context->AddActionContext(new MovementActionContext());
    }
}
