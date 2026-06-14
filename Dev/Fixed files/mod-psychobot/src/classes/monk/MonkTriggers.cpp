/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "MonkTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../engine/ServerFacade.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_CHI, POWER_ENERGY
#include <string>

namespace psychobot
{
    bool ChiAboveTrigger::IsActive()
    {
        // Raw Chi count (0..5/6).
        return ServerFacade::GetPower(GetBot(), POWER_CHI) >= _count;
    }

    bool MonkEnergyAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<float>* e = ctx->GetValue<float>("power", std::to_string(POWER_ENERGY));
        return e && e->Get() >= _pct;
    }

    bool MonkTargetHealthBelowTrigger::IsActive()
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

    bool MonkSelfMissingAuraTrigger::IsActive()
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
        class MonkTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            MonkTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["chi high"]   = [](PsychobotAI* ai) -> Trigger* { return new ChiAboveTrigger(ai, "chi high", 4); };
                _creators["chi enough"] = [](PsychobotAI* ai) -> Trigger* { return new ChiAboveTrigger(ai, "chi enough", 2); };

                _creators["energy high"] = [](PsychobotAI* ai) -> Trigger* { return new MonkEnergyAboveTrigger(ai, "energy high", 70.0f); };

                _creators["target low health"] = [](PsychobotAI* ai) -> Trigger* { return new MonkTargetHealthBelowTrigger(ai, "target low health", 15.0f); };

                _creators["ironskin brew absent"] = [](PsychobotAI* ai) -> Trigger* { return new MonkSelfMissingAuraTrigger(ai, "ironskin brew absent", "Ironskin Brew"); };
                _creators["shuffle absent"]       = [](PsychobotAI* ai) -> Trigger* { return new MonkSelfMissingAuraTrigger(ai, "shuffle absent", "Shuffle"); };
            }
        };
    }

    void RegisterMonkTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new MonkTriggerContext());
    }
}
