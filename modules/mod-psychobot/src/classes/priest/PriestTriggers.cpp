/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PriestTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../engine/ServerFacade.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_INSANITY
#include <string>

namespace psychobot
{
    bool InsanityAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<float>* ins = ctx->GetValue<float>("power", std::to_string(POWER_INSANITY));
        return ins && ins->Get() >= _pct;
    }

    bool PriestTargetHealthBelowTrigger::IsActive()
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

    bool PriestTargetMissingAuraTrigger::IsActive()
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

    bool PriestSelfMissingAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<bool>* has = ctx->GetValue<bool>("has aura", _auraName);
        return has && !has->Get();
    }

    bool HealTargetMissingAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        // The most-injured group member (or self solo).
        Value<Unit*>* v = ctx->GetValue<Unit*>("party member to heal");
        Unit* healTarget = v ? v->Get() : nullptr;
        if (!healTarget)
            return false;
        // Resolve the Atonement spell id by name and check the heal target.
        uint32 spellId = PsychobotAIBridge::GetSpellId(GetAI(), _auraName);
        if (!spellId)
            return false;   // bot doesn't know it -> nothing to apply
        return !ServerFacade::HasAura(healTarget, spellId);
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class PriestTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            PriestTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["insanity high"] = [](PsychobotAI* ai) -> Trigger* { return new InsanityAboveTrigger(ai, "insanity high", 80.0f); };

                _creators["target low health"] = [](PsychobotAI* ai) -> Trigger* { return new PriestTargetHealthBelowTrigger(ai, "target low health", 20.0f); };

                _creators["shadow word pain absent"] = [](PsychobotAI* ai) -> Trigger* { return new PriestTargetMissingAuraTrigger(ai, "shadow word pain absent", "Shadow Word: Pain"); };
                _creators["vampiric touch absent"]   = [](PsychobotAI* ai) -> Trigger* { return new PriestTargetMissingAuraTrigger(ai, "vampiric touch absent", "Vampiric Touch"); };

                _creators["shadowform absent"] = [](PsychobotAI* ai) -> Trigger* { return new PriestSelfMissingAuraTrigger(ai, "shadowform absent", "Shadowform"); };

                _creators["atonement low"] = [](PsychobotAI* ai) -> Trigger* { return new HealTargetMissingAuraTrigger(ai, "atonement low", "Atonement"); };
            }
        };
    }

    void RegisterPriestTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new PriestTriggerContext());
    }
}
