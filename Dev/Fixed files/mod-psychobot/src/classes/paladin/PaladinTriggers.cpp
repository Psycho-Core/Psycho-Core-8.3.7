/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PaladinTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../engine/ServerFacade.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_HOLY_POWER
#include <string>

namespace psychobot
{
    bool HolyPowerAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        // "power::9" = Holy Power percent (POWER_HOLY_POWER = 9, max 5).
        Value<float>* hp = ctx->GetValue<float>("power", std::to_string(POWER_HOLY_POWER));
        return hp && hp->Get() >= _pct;
    }

    bool PaladinTargetHealthBelowTrigger::IsActive()
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

    bool PaladinSelfMissingAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<bool>* has = ctx->GetValue<bool>("has aura", _auraName);
        return has && !has->Get();
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class PaladinTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            PaladinTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                // 3 of 5 Holy Power ~ 60%.
                _creators["holy power high"] = [](PsychobotAI* ai) -> Trigger* { return new HolyPowerAboveTrigger(ai, "holy power high", 60.0f); };

                _creators["target low health"] = [](PsychobotAI* ai) -> Trigger* { return new PaladinTargetHealthBelowTrigger(ai, "target low health", 20.0f); };

                _creators["devotion aura absent"]       = [](PsychobotAI* ai) -> Trigger* { return new PaladinSelfMissingAuraTrigger(ai, "devotion aura absent", "Devotion Aura"); };
                _creators["shield of righteous absent"] = [](PsychobotAI* ai) -> Trigger* { return new PaladinSelfMissingAuraTrigger(ai, "shield of righteous absent", "Shield of the Righteous"); };
            }
        };
    }

    void RegisterPaladinTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new PaladinTriggerContext());
    }
}
