/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "DruidTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../engine/ServerFacade.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_LUNAR_POWER, POWER_RAGE, POWER_COMBO_POINTS
#include <string>

namespace psychobot
{
    bool DruidPowerAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<float>* p = ctx->GetValue<float>("power", std::to_string(_powerType));
        return p && p->Get() >= _pct;
    }

    bool DruidComboPointsAboveTrigger::IsActive()
    {
        return ServerFacade::GetPower(GetBot(), POWER_COMBO_POINTS) >= _count;
    }

    bool DruidTargetMissingAuraTrigger::IsActive()
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

    bool DruidSelfMissingAuraTrigger::IsActive()
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
        class DruidTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            DruidTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["astral power high"] = [](PsychobotAI* ai) -> Trigger* { return new DruidPowerAboveTrigger(ai, "astral power high", POWER_LUNAR_POWER, 80.0f); };
                _creators["rage high"]         = [](PsychobotAI* ai) -> Trigger* { return new DruidPowerAboveTrigger(ai, "rage high", POWER_RAGE, 70.0f); };

                _creators["combo points full"] = [](PsychobotAI* ai) -> Trigger* { return new DruidComboPointsAboveTrigger(ai, "combo points full", 5); };

                _creators["moonkin form absent"] = [](PsychobotAI* ai) -> Trigger* { return new DruidSelfMissingAuraTrigger(ai, "moonkin form absent", "Moonkin Form"); };
                _creators["cat form absent"]     = [](PsychobotAI* ai) -> Trigger* { return new DruidSelfMissingAuraTrigger(ai, "cat form absent", "Cat Form"); };
                _creators["bear form absent"]    = [](PsychobotAI* ai) -> Trigger* { return new DruidSelfMissingAuraTrigger(ai, "bear form absent", "Bear Form"); };
                _creators["ironfur absent"]      = [](PsychobotAI* ai) -> Trigger* { return new DruidSelfMissingAuraTrigger(ai, "ironfur absent", "Ironfur"); };

                _creators["moonfire absent"] = [](PsychobotAI* ai) -> Trigger* { return new DruidTargetMissingAuraTrigger(ai, "moonfire absent", "Moonfire"); };
                _creators["sunfire absent"]  = [](PsychobotAI* ai) -> Trigger* { return new DruidTargetMissingAuraTrigger(ai, "sunfire absent", "Sunfire"); };
                _creators["rake absent"]     = [](PsychobotAI* ai) -> Trigger* { return new DruidTargetMissingAuraTrigger(ai, "rake absent", "Rake"); };
                _creators["rip absent"]      = [](PsychobotAI* ai) -> Trigger* { return new DruidTargetMissingAuraTrigger(ai, "rip absent", "Rip"); };
                _creators["thrash absent"]   = [](PsychobotAI* ai) -> Trigger* { return new DruidTargetMissingAuraTrigger(ai, "thrash absent", "Thrash"); };
            }
        };
    }

    void RegisterDruidTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new DruidTriggerContext());
    }
}
