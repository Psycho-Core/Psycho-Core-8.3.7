/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "DKTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_RUNIC_POWER
#include <string>

namespace psychobot
{
    bool RunicPowerAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        // "power::6" = Runic Power percent (POWER_RUNIC_POWER = 6).
        Value<float>* rp = ctx->GetValue<float>("power", std::to_string(POWER_RUNIC_POWER));
        return rp && rp->Get() >= _pct;
    }

    bool TargetMissingAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        // Only meaningful with a target.
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        if (!t || !t->Get())
            return false;
        Value<bool>* has = ctx->GetValue<bool>("target has aura", _auraName);
        return has && !has->Get();
    }

    bool SelfHasAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<bool>* has = ctx->GetValue<bool>("has aura", _auraName);
        return has && has->Get();
    }

    bool SelfMissingAuraTrigger::IsActive()
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
        class DKTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            DKTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["runic power high"]   = [](PsychobotAI* ai) -> Trigger* { return new RunicPowerAboveTrigger(ai, "runic power high", 80.0f); };
                _creators["runic power enough"] = [](PsychobotAI* ai) -> Trigger* { return new RunicPowerAboveTrigger(ai, "runic power enough", 35.0f); };

                _creators["frost fever absent"]     = [](PsychobotAI* ai) -> Trigger* { return new TargetMissingAuraTrigger(ai, "frost fever absent", "Frost Fever"); };
                _creators["blood plague absent"]    = [](PsychobotAI* ai) -> Trigger* { return new TargetMissingAuraTrigger(ai, "blood plague absent", "Blood Plague"); };
                _creators["virulent plague absent"] = [](PsychobotAI* ai) -> Trigger* { return new TargetMissingAuraTrigger(ai, "virulent plague absent", "Virulent Plague"); };

                _creators["killing machine"] = [](PsychobotAI* ai) -> Trigger* { return new SelfHasAuraTrigger(ai, "killing machine", "Killing Machine"); };
                _creators["rime"]            = [](PsychobotAI* ai) -> Trigger* { return new SelfHasAuraTrigger(ai, "rime", "Rime"); };
                _creators["sudden doom"]     = [](PsychobotAI* ai) -> Trigger* { return new SelfHasAuraTrigger(ai, "sudden doom", "Sudden Doom"); };

                _creators["bone shield low"] = [](PsychobotAI* ai) -> Trigger* { return new SelfMissingAuraTrigger(ai, "bone shield low", "Bone Shield"); };
            }
        };
    }

    void RegisterDeathKnightTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new DKTriggerContext());
    }
}
