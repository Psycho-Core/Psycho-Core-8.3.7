/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "MageTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../PsychobotAIFwd.h"
#include "../../engine/ServerFacade.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_ARCANE_CHARGES
#include <string>

namespace psychobot
{
    bool ArcaneChargesAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<float>* ac = ctx->GetValue<float>("power", std::to_string(POWER_ARCANE_CHARGES));
        return ac && ac->Get() >= _pct;
    }

    bool MageSelfHasAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<bool>* has = ctx->GetValue<bool>("has aura", _auraName);
        return has && has->Get();
    }

    bool MageTargetHealthBelowTrigger::IsActive()
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

    bool MageSelfMissingAuraTrigger::IsActive()
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
        class MageTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            MageTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["arcane charges high"] = [](PsychobotAI* ai) -> Trigger* { return new ArcaneChargesAboveTrigger(ai, "arcane charges high", 80.0f); };

                _creators["target low health"] = [](PsychobotAI* ai) -> Trigger* { return new MageTargetHealthBelowTrigger(ai, "target low health", 30.0f); };

                _creators["clearcasting"]     = [](PsychobotAI* ai) -> Trigger* { return new MageSelfHasAuraTrigger(ai, "clearcasting", "Clearcasting"); };
                _creators["hot streak"]       = [](PsychobotAI* ai) -> Trigger* { return new MageSelfHasAuraTrigger(ai, "hot streak", "Hot Streak"); };
                _creators["brain freeze"]     = [](PsychobotAI* ai) -> Trigger* { return new MageSelfHasAuraTrigger(ai, "brain freeze", "Brain Freeze"); };
                _creators["fingers of frost"] = [](PsychobotAI* ai) -> Trigger* { return new MageSelfHasAuraTrigger(ai, "fingers of frost", "Fingers of Frost"); };

                _creators["arcane intellect absent"] = [](PsychobotAI* ai) -> Trigger* { return new MageSelfMissingAuraTrigger(ai, "arcane intellect absent", "Arcane Intellect"); };
            }
        };
    }

    void RegisterMageTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new MageTriggerContext());
    }
}
