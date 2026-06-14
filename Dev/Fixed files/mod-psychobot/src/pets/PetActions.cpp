/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PetActions.h"
#include "../engine/AiObjectContext.h"
#include "../engine/ServerFacade.h"
#include "../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"

namespace psychobot
{
    // ----------------------------------------------------------------------
    // PetAttackAction
    // ----------------------------------------------------------------------
    Unit* PetAttackAction::GetTarget()
    {
        return PsychobotAIBridge::GetCurrentTarget(GetAI());
    }

    bool PetAttackAction::IsUseful()
    {
        // Useful only if we have a living pet AND a valid target it isn't on yet.
        Player* bot = GetBot();
        if (!ServerFacade::HasLivingPet(bot))
            return false;
        Unit* target = GetTarget();
        if (!ServerFacade::IsValidAttackTarget(bot, target))
            return false;
        return !ServerFacade::PetAttackingTarget(bot, target);
    }

    bool PetAttackAction::Execute(Event const& /*event*/)
    {
        return PsychobotAIBridge::PetAttack(GetAI(), GetTarget());
    }

    // ----------------------------------------------------------------------
    // PetFollowAction
    // ----------------------------------------------------------------------
    bool PetFollowAction::IsUseful()
    {
        return ServerFacade::HasLivingPet(GetBot());
    }

    bool PetFollowAction::Execute(Event const& /*event*/)
    {
        return PsychobotAIBridge::PetFollow(GetAI());
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class PetActionContext : public NamedObjectContext<Action>
        {
        public:
            PetActionContext() : NamedObjectContext<Action>(/*shared*/ false)
            {
                _creators["pet attack"] = [](PsychobotAI* ai) -> Action* { return new PetAttackAction(ai); };
                _creators["pet follow"] = [](PsychobotAI* ai) -> Action* { return new PetFollowAction(ai); };
            }
        };
    }

    void RegisterPetActions(AiObjectContext* context)
    {
        if (context)
            context->AddActionContext(new PetActionContext());
    }
}
