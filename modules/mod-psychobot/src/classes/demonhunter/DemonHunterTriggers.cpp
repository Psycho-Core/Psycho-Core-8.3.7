/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "DemonHunterTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_FURY, POWER_PAIN
#include <string>

namespace psychobot
{
    bool DHPowerAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<float>* p = ctx->GetValue<float>("power", std::to_string(_powerType));
        return p && p->Get() >= _pct;
    }

    bool DHSelfHasAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<bool>* has = ctx->GetValue<bool>("has aura", _auraName);
        return has && has->Get();
    }

    bool DHSelfMissingAuraTrigger::IsActive()
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
        class DHTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            DHTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["fury high"] = [](PsychobotAI* ai) -> Trigger* { return new DHPowerAboveTrigger(ai, "fury high", POWER_FURY, 70.0f); };
                _creators["pain high"] = [](PsychobotAI* ai) -> Trigger* { return new DHPowerAboveTrigger(ai, "pain high", POWER_PAIN, 70.0f); };

                _creators["soul fragments high"] = [](PsychobotAI* ai) -> Trigger* { return new DHSelfHasAuraTrigger(ai, "soul fragments high", "Soul Fragments"); };

                _creators["immolation aura absent"] = [](PsychobotAI* ai) -> Trigger* { return new DHSelfMissingAuraTrigger(ai, "immolation aura absent", "Immolation Aura"); };
                _creators["demon spikes absent"]    = [](PsychobotAI* ai) -> Trigger* { return new DHSelfMissingAuraTrigger(ai, "demon spikes absent", "Demon Spikes"); };
            }
        };
    }

    void RegisterDemonHunterTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new DHTriggerContext());
    }
}
