/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "RogueTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../engine/ServerFacade.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_ENERGY, POWER_COMBO_POINTS
#include <string>

namespace psychobot
{
    bool ComboPointsAboveTrigger::IsActive()
    {
        // Raw combo-point count (0..5).
        return ServerFacade::GetPower(GetBot(), POWER_COMBO_POINTS) >= _count;
    }

    bool ComboPointsBelowTrigger::IsActive()
    {
        return ServerFacade::GetPower(GetBot(), POWER_COMBO_POINTS) <= _count;
    }

    bool EnergyAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<float>* e = ctx->GetValue<float>("power", std::to_string(POWER_ENERGY));
        return e && e->Get() >= _pct;
    }

    bool StealthedTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        // Either Stealth or Shadow Dance counts as "stealthed" for openers.
        Value<bool>* st = ctx->GetValue<bool>("has aura", "Stealth");
        Value<bool>* sd = ctx->GetValue<bool>("has aura", "Shadow Dance");
        bool stealthed = (st && st->Get()) || (sd && sd->Get());
        return _want ? stealthed : !stealthed;
    }

    bool RogueTargetHealthBelowTrigger::IsActive()
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

    bool RogueTargetMissingAuraTrigger::IsActive()
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

    bool RogueSelfMissingAuraTrigger::IsActive()
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
        class RogueTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            RogueTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["combo points full"] = [](PsychobotAI* ai) -> Trigger* { return new ComboPointsAboveTrigger(ai, "combo points full", 5); };
                _creators["combo points low"]  = [](PsychobotAI* ai) -> Trigger* { return new ComboPointsBelowTrigger(ai, "combo points low", 1); };

                _creators["energy high"] = [](PsychobotAI* ai) -> Trigger* { return new EnergyAboveTrigger(ai, "energy high", 70.0f); };

                _creators["stealthed"]     = [](PsychobotAI* ai) -> Trigger* { return new StealthedTrigger(ai, "stealthed", true); };
                _creators["not stealthed"] = [](PsychobotAI* ai) -> Trigger* { return new StealthedTrigger(ai, "not stealthed", false); };

                _creators["target low health"] = [](PsychobotAI* ai) -> Trigger* { return new RogueTargetHealthBelowTrigger(ai, "target low health", 35.0f); };

                _creators["rupture absent"] = [](PsychobotAI* ai) -> Trigger* { return new RogueTargetMissingAuraTrigger(ai, "rupture absent", "Rupture"); };
                _creators["garrote absent"] = [](PsychobotAI* ai) -> Trigger* { return new RogueTargetMissingAuraTrigger(ai, "garrote absent", "Garrote"); };

                _creators["slice and dice absent"] = [](PsychobotAI* ai) -> Trigger* { return new RogueSelfMissingAuraTrigger(ai, "slice and dice absent", "Slice and Dice"); };
            }
        };
    }

    void RegisterRogueTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new RogueTriggerContext());
    }
}
