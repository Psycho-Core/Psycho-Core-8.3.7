/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PetStrategy.h"
#include "PetActions.h"
#include "../engine/AiObjectContext.h"
#include "../engine/ServerFacade.h"
#include "../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"

namespace psychobot
{
    bool PetIdleTrigger::IsActive()
    {
        Player* bot = GetBot();
        if (!ServerFacade::HasLivingPet(bot))
            return false;
        Unit* target = PsychobotAIBridge::GetCurrentTarget(GetAI());
        if (!ServerFacade::IsValidAttackTarget(bot, target))
            return false;
        // Fire only when the pet is NOT already on the target.
        return !ServerFacade::PetAttackingTarget(bot, target);
    }

    void PetStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
    {
        // Direct the pet onto the current target (high so it lands promptly,
        // but it's a cheap, free action that doesn't consume the bot's GCD).
        triggers.push_back(new TriggerNode("pet idle",
            NextAction::Array(1, new NextAction("pet attack", ACTION_HIGH), nullptr)));
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class PetTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            PetTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["pet idle"] = [](PsychobotAI* ai) -> Trigger* { return new PetIdleTrigger(ai); };
            }
        };

        class PetStrategyContext : public NamedObjectContext<Strategy>
        {
        public:
            PetStrategyContext() : NamedObjectContext<Strategy>(/*shared*/ false)
            {
                _creators["pet"] = [](PsychobotAI* ai) -> Strategy* { return new PetStrategy(ai); };
            }
        };
    }

    void RegisterPetSystem(AiObjectContext* context)
    {
        if (!context)
            return;
        RegisterPetActions(context);
        context->AddTriggerContext(new PetTriggerContext());
        context->AddStrategyContext(new PetStrategyContext());
    }
}
