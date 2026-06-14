/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "WarriorTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../engine/ServerFacade.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_RAGE
#include <string>

namespace psychobot
{
    bool RageAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        // "power::1" = Rage percent (POWER_RAGE = 1).
        Value<float>* rage = ctx->GetValue<float>("power", std::to_string(POWER_RAGE));
        return rage && rage->Get() >= _pct;
    }

    bool TargetHealthBelowTrigger::IsActive()
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

    bool WarriorSelfHasAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<bool>* has = ctx->GetValue<bool>("has aura", _auraName);
        return has && has->Get();
    }

    bool WarriorSelfMissingAuraTrigger::IsActive()
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
        class WarriorTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            WarriorTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["rage high"]   = [](PsychobotAI* ai) -> Trigger* { return new RageAboveTrigger(ai, "rage high", 80.0f); };
                _creators["rage enough"] = [](PsychobotAI* ai) -> Trigger* { return new RageAboveTrigger(ai, "rage enough", 30.0f); };

                _creators["target low health"] = [](PsychobotAI* ai) -> Trigger* { return new TargetHealthBelowTrigger(ai, "target low health", 20.0f); };

                _creators["sudden death"]   = [](PsychobotAI* ai) -> Trigger* { return new WarriorSelfHasAuraTrigger(ai, "sudden death", "Sudden Death"); };
                _creators["overpower ready"]= [](PsychobotAI* ai) -> Trigger* { return new WarriorSelfHasAuraTrigger(ai, "overpower ready", "Overpower"); };

                _creators["enrage absent"]  = [](PsychobotAI* ai) -> Trigger* { return new WarriorSelfMissingAuraTrigger(ai, "enrage absent", "Enrage"); };
                _creators["battle stance absent"]    = [](PsychobotAI* ai) -> Trigger* { return new WarriorSelfMissingAuraTrigger(ai, "battle stance absent", "Battle Stance"); };
                _creators["defensive stance absent"] = [](PsychobotAI* ai) -> Trigger* { return new WarriorSelfMissingAuraTrigger(ai, "defensive stance absent", "Defensive Stance"); };
                _creators["shield block absent"]     = [](PsychobotAI* ai) -> Trigger* { return new WarriorSelfMissingAuraTrigger(ai, "shield block absent", "Shield Block"); };
            }
        };
    }

    void RegisterWarriorTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new WarriorTriggerContext());
    }
}
