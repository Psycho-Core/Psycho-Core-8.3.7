/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "ShamanTriggers.h"
#include "../../engine/AiObjectContext.h"
#include "../../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_MAELSTROM
#include <string>

namespace psychobot
{
    bool MaelstromAboveTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<float>* m = ctx->GetValue<float>("power", std::to_string(POWER_MAELSTROM));
        return m && m->Get() >= _pct;
    }

    bool ShamanTargetMissingAuraTrigger::IsActive()
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

    bool ShamanSelfHasAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return false;
        Value<bool>* has = ctx->GetValue<bool>("has aura", _auraName);
        return has && has->Get();
    }

    bool ShamanSelfMissingAuraTrigger::IsActive()
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
        class ShamanTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            ShamanTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["maelstrom high"] = [](PsychobotAI* ai) -> Trigger* { return new MaelstromAboveTrigger(ai, "maelstrom high", 80.0f); };

                _creators["flame shock absent"] = [](PsychobotAI* ai) -> Trigger* { return new ShamanTargetMissingAuraTrigger(ai, "flame shock absent", "Flame Shock"); };

                _creators["maelstrom weapon high"] = [](PsychobotAI* ai) -> Trigger* { return new ShamanSelfHasAuraTrigger(ai, "maelstrom weapon high", "Maelstrom Weapon"); };

                _creators["windfury weapon absent"]     = [](PsychobotAI* ai) -> Trigger* { return new ShamanSelfMissingAuraTrigger(ai, "windfury weapon absent", "Windfury Weapon"); };
                _creators["flametongue weapon absent"]  = [](PsychobotAI* ai) -> Trigger* { return new ShamanSelfMissingAuraTrigger(ai, "flametongue weapon absent", "Flametongue Weapon"); };
                _creators["lightning shield absent"]    = [](PsychobotAI* ai) -> Trigger* { return new ShamanSelfMissingAuraTrigger(ai, "lightning shield absent", "Lightning Shield"); };
            }
        };
    }

    void RegisterShamanTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new ShamanTriggerContext());
    }
}
