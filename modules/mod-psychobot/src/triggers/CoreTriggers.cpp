/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "CoreTriggers.h"
#include "../engine/AiObjectContext.h"
#include "../engine/ServerFacade.h"
#include "../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"

namespace psychobot
{
    AiObjectContext* ContextTrigger::Context() const
    {
        return PsychobotAIBridge::GetContext(GetAI());
    }

    // --- health -----------------------------------------------------------
    bool HealthBelowTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<float>* hp = ctx->GetValue<float>("health");
        return hp && hp->Get() < _pct && hp->Get() > 0.0f;
    }

    // --- combat / target --------------------------------------------------
    bool HasTargetTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        Unit* target = t ? t->Get() : nullptr;
        return ServerFacade::IsValidAttackTarget(GetBot(), target);
    }

    bool NoTargetTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return true;
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        Unit* target = t ? t->Get() : nullptr;
        return !ServerFacade::IsValidAttackTarget(GetBot(), target);
    }

    bool EnemyInMeleeTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        Unit* target = t ? t->Get() : nullptr;
        return target && ServerFacade::IsInMeleeRange(GetBot(), target);
    }

    bool EnemyOutOfMeleeTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        Unit* target = t ? t->Get() : nullptr;
        return target && ServerFacade::IsValidAttackTarget(GetBot(), target)
            && !ServerFacade::IsInMeleeRange(GetBot(), target);
    }

    bool TargetCastingTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        Unit* target = t ? t->Get() : nullptr;
        return target && ServerFacade::IsCasting(target);
    }

    // --- auras (qualified) ------------------------------------------------
    bool HasAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<bool>* v = ctx->GetValue<bool>("has aura", GetQualifier());
        return v && v->Get();
    }

    bool NoAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<bool>* v = ctx->GetValue<bool>("has aura", GetQualifier());
        return v && !v->Get();
    }

    bool TargetNoAuraTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        // Only meaningful when there is a target.
        Value<Unit*>* t = ctx->GetValue<Unit*>("current target");
        Unit* target = t ? t->Get() : nullptr;
        if (!target)
            return false;
        Value<bool>* v = ctx->GetValue<bool>("target has aura", GetQualifier());
        return v && !v->Get();
    }

    // --- spell ready ------------------------------------------------------
    bool SpellReadyTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<bool>* v = ctx->GetValue<bool>("spell ready", GetQualifier());
        return v && v->Get();
    }

    // --- group ------------------------------------------------------------
    bool PartyMemberLowHealthTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<Unit*>* v = ctx->GetValue<Unit*>("party member to heal");
        return v && v->Get() != nullptr;
    }

    bool PartyMemberNeedsDispelTrigger::IsActive()
    {
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<Unit*>* v = ctx->GetValue<Unit*>("party member to dispel");
        return v && v->Get() != nullptr;
    }

    bool LowManaTrigger::IsActive()
    {
        // Mana is Powers index 0; "power::0" is the bot's mana percent.
        AiObjectContext* ctx = Context();
        if (!ctx)
            return false;
        Value<float>* m = ctx->GetValue<float>("power", "0");
        // Only meaningful for mana users; non-mana classes read 0% and would
        // always trip, so require a real mana bar via ServerFacade.
        if (ServerFacade::GetManaPct(GetBot()) <= 0.0f)
            return false;
        return m && m->Get() < _pct;
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class CoreTriggerContext : public NamedObjectContext<Trigger>
        {
        public:
            CoreTriggerContext() : NamedObjectContext<Trigger>(/*shared*/ false)
            {
                _creators["low health"]      = [](PsychobotAI* ai) -> Trigger* { return new HealthBelowTrigger(ai, "low health", 50.0f); };
                _creators["medium health"]   = [](PsychobotAI* ai) -> Trigger* { return new HealthBelowTrigger(ai, "medium health", 70.0f); };
                _creators["critical health"] = [](PsychobotAI* ai) -> Trigger* { return new HealthBelowTrigger(ai, "critical health", 30.0f); };

                _creators["has target"]         = [](PsychobotAI* ai) -> Trigger* { return new HasTargetTrigger(ai); };
                _creators["no target"]          = [](PsychobotAI* ai) -> Trigger* { return new NoTargetTrigger(ai); };
                _creators["enemy is close"]     = [](PsychobotAI* ai) -> Trigger* { return new EnemyInMeleeTrigger(ai); };
                _creators["enemy out of melee"] = [](PsychobotAI* ai) -> Trigger* { return new EnemyOutOfMeleeTrigger(ai); };
                _creators["target casting"]     = [](PsychobotAI* ai) -> Trigger* { return new TargetCastingTrigger(ai); };

                _creators["has aura"]            = [](PsychobotAI* ai) -> Trigger* { return new HasAuraTrigger(ai); };
                _creators["no aura"]             = [](PsychobotAI* ai) -> Trigger* { return new NoAuraTrigger(ai); };
                _creators["target has no aura"]  = [](PsychobotAI* ai) -> Trigger* { return new TargetNoAuraTrigger(ai); };
                _creators["spell ready"]         = [](PsychobotAI* ai) -> Trigger* { return new SpellReadyTrigger(ai); };

                _creators["party member low health"] = [](PsychobotAI* ai) -> Trigger* { return new PartyMemberLowHealthTrigger(ai); };
                _creators["party member needs dispel"] = [](PsychobotAI* ai) -> Trigger* { return new PartyMemberNeedsDispelTrigger(ai); };
                _creators["low mana"] = [](PsychobotAI* ai) -> Trigger* { return new LowManaTrigger(ai, 25.0f); };
            }
        };
    }

    void RegisterCoreTriggers(AiObjectContext* context)
    {
        if (context)
            context->AddTriggerContext(new CoreTriggerContext());
    }
}
