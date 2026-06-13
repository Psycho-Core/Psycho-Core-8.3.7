/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "WarlockTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../engine/ServerFacade.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_SOUL_SHARDS
#include <string>

namespace psychobot
{
    bool SoulShardsAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<float>* ss = ctx->GetValue<float>("power", std::to_string(POWER_SOUL_SHARDS));
        return ss && ss->Get() >= _pct;
    }

    bool WarlockTargetMissingAuraTrigger::IsActive()
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

    bool WarlockSelfHasAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<bool>* has = ctx->GetValue<bool>("has aura", _auraName);
        return has && has->Get();
    }

    bool NoWarlockPetTrigger::IsActive()
    {
        return !ServerFacade::HasLivingPet(GetBot());
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class WarlockTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            WarlockTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["soul shards high"] = [](PsychobotAI* ai) -> Trigger* { return new SoulShardsAboveTrigger(ai, "soul shards high", 60.0f); };

                _creators["agony absent"]      = [](PsychobotAI* ai) -> Trigger* { return new WarlockTargetMissingAuraTrigger(ai, "agony absent", "Agony"); };
                _creators["corruption absent"] = [](PsychobotAI* ai) -> Trigger* { return new WarlockTargetMissingAuraTrigger(ai, "corruption absent", "Corruption"); };
                _creators["immolate absent"]   = [](PsychobotAI* ai) -> Trigger* { return new WarlockTargetMissingAuraTrigger(ai, "immolate absent", "Immolate"); };

                _creators["demonic core"] = [](PsychobotAI* ai) -> Trigger* { return new WarlockSelfHasAuraTrigger(ai, "demonic core", "Demonic Core"); };

                _creators["no warlock pet"] = [](PsychobotAI* ai) -> Trigger* { return new NoWarlockPetTrigger(ai); };
            }
        };
    }

    void RegisterWarlockTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new WarlockTriggerContext());
    }
}
