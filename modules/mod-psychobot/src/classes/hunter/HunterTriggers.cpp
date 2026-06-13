/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "HunterTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../engine/ServerFacade.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_FOCUS
#include <string>

namespace psychobot
{
    bool FocusAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        // "power::2" = Focus percent (POWER_FOCUS = 2).
        Value<float>* focus = ctx->GetValue<float>("power", std::to_string(POWER_FOCUS));
        return focus && focus->Get() >= _pct;
    }

    bool HunterTargetHealthBelowTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        Unit* target = t ? t->Get() : nullptr;
        if (!target)
            return false;
        float pct = ServerFacade::GetHealthPct(target);
        return pct > 0.0f && pct <= _pct;
    }

    bool NoPetTrigger::IsActive()
    {
        // Only relevant in combat (don't auto-summon while idle/out of range
        // checks); the strategy gates this with "has target" too.
        return !ServerFacade::HasLivingPet(GetBot());
    }

    bool PetHurtTrigger::IsActive()
    {
        Player* bot = GetBot();
        if (!ServerFacade::HasLivingPet(bot))
            return false;
        float pct = ServerFacade::GetPetHealthPct(bot);
        return pct > 0.0f && pct < _pct;
    }

    bool HunterTargetMissingAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        if (!t || !t->Get())
            return false;
        Value<bool>* has = ctx->GetValue<bool>("target has aura", _auraName);
        return has && !has->Get();
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class HunterTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            HunterTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["focus high"]   = [](PsychobotAI* ai) -> Trigger* { return new FocusAboveTrigger(ai, "focus high", 70.0f); };
                _creators["focus enough"] = [](PsychobotAI* ai) -> Trigger* { return new FocusAboveTrigger(ai, "focus enough", 40.0f); };

                _creators["target low health"] = [](PsychobotAI* ai) -> Trigger* { return new HunterTargetHealthBelowTrigger(ai, "target low health", 20.0f); };

                _creators["no pet"]   = [](PsychobotAI* ai) -> Trigger* { return new NoPetTrigger(ai); };
                _creators["pet hurt"] = [](PsychobotAI* ai) -> Trigger* { return new PetHurtTrigger(ai, 60.0f); };

                _creators["serpent sting absent"] = [](PsychobotAI* ai) -> Trigger* { return new HunterTargetMissingAuraTrigger(ai, "serpent sting absent", "Serpent Sting"); };
            }
        };
    }

    void RegisterHunterTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new HunterTriggerContext());
    }
}
