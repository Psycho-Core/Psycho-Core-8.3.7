/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "GenericSpellActions.h"
#include "../engine/AiObjectContext.h"
#include "../engine/ServerFacade.h"
#include "../PsychobotAIFwd.h"
#include "Player.h"
#include "Unit.h"

namespace psychobot
{
    // ----------------------------------------------------------------------
    // CastSpellAction
    // ----------------------------------------------------------------------
    uint32 CastSpellAction::ResolveSpell()
    {
        return PsychobotAIBridge::GetSpellId(GetAI(), GetQualifier());
    }

    Unit* CastSpellAction::GetTarget()
    {
        return PsychobotAIBridge::GetCurrentTarget(GetAI());
    }

    float CastSpellAction::GetCastRange()
    {
        return 30.0f;   // generic ranged spell range
    }

    bool CastSpellAction::IsUseful()
    {
        // Useful if the bot knows the spell and there's a valid target.
        if (!ResolveSpell())
            return false;
        Unit* target = GetTarget();
        return ServerFacade::IsValidAttackTarget(GetBot(), target);
    }

    bool CastSpellAction::IsPossible()
    {
        uint32 spellId = ResolveSpell();
        if (!spellId)
            return false;
        // Known + off cooldown + not already casting.
        if (!ServerFacade::IsSpellReady(GetBot(), spellId))
            return false;
        if (ServerFacade::IsCasting(GetBot()))
            return false;
        return true;
    }

    NextAction** CastSpellAction::GetPrerequisites()
    {
        std::string reach = GetReachActionName();
        if (reach.empty())
            return nullptr;
        return NextAction::Array(1, new NextAction(reach, 0.0f), nullptr);
    }

    bool CastSpellAction::Execute(Event const& /*event*/)
    {
        uint32 spellId = ResolveSpell();
        if (!spellId)
            return false;

        Unit* target = GetTarget();
        if (!target)
            return false;

        // Range gate: if out of cast range, the prerequisite "reach" action
        // should have run first; bail so the engine re-queues it.
        float range = GetCastRange();
        if (range > 0.0f && !ServerFacade::IsInRange(GetBot(), target, range))
            return false;

        return PsychobotAIBridge::CastSpell(GetAI(), spellId, target);
    }

    // ----------------------------------------------------------------------
    // CastMeleeSpellAction
    // ----------------------------------------------------------------------
    float CastMeleeSpellAction::GetCastRange()
    {
        return 5.0f;    // melee reach (refined by IsInMeleeRange in Execute)
    }

    // ----------------------------------------------------------------------
    // CastAuraSpellAction - only if target lacks the aura.
    // ----------------------------------------------------------------------
    bool CastAuraSpellAction::IsUseful()
    {
        if (!CastSpellAction::IsUseful())
            return false;

        uint32 spellId = ResolveSpell();
        Unit* target = GetTarget();
        // Don't re-apply if the target already has this aura.
        return target && !ServerFacade::HasAura(target, spellId);
    }

    // ----------------------------------------------------------------------
    // CastBuffSpellAction - self-buff if missing.
    // ----------------------------------------------------------------------
    Unit* CastBuffSpellAction::GetTarget()
    {
        return GetBot();   // self
    }

    bool CastBuffSpellAction::IsUseful()
    {
        uint32 spellId = ResolveSpell();
        if (!spellId)
            return false;
        return !ServerFacade::HasAura(GetBot(), spellId);
    }

    // ----------------------------------------------------------------------
    // CastHealSpellAction - heal the "party member to heal" value.
    // ----------------------------------------------------------------------
    Unit* CastHealSpellAction::GetTarget()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return nullptr;
        Value<Unit*>* v = ctx->GetValue<Unit*>("party member to heal");
        return v ? v->Get() : nullptr;
    }

    bool CastHealSpellAction::IsUseful()
    {
        if (!ResolveSpell())
            return false;
        return GetTarget() != nullptr;   // someone needs healing
    }

    // ----------------------------------------------------------------------
    // CastDispelSpellAction - cure the "party member to dispel" value.
    // ----------------------------------------------------------------------
    Unit* CastDispelSpellAction::GetTarget()
    {
        AiObjectContext* ctx = PsychobotAIBridge::GetContext(GetAI());
        if (!ctx)
            return nullptr;
        Value<Unit*>* v = ctx->GetValue<Unit*>("party member to dispel");
        return v ? v->Get() : nullptr;
    }

    bool CastDispelSpellAction::IsUseful()
    {
        if (!ResolveSpell())
            return false;
        return GetTarget() != nullptr;   // someone has a removable debuff
    }

    // ----------------------------------------------------------------------
    // CastSelfSpellAction - self-cast summon/cooldown (no target/aura gate).
    // ----------------------------------------------------------------------
    Unit* CastSelfSpellAction::GetTarget()
    {
        return GetBot();   // self
    }

    bool CastSelfSpellAction::IsUseful()
    {
        // Useful purely on knowing the spell; spell-ready (CD) is the real gate
        // and is checked in IsPossible().
        return ResolveSpell() != 0;
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class GenericSpellActionContext : public NamedObjectContext<Action>
        {
        public:
            GenericSpellActionContext() : NamedObjectContext<Action>(/*shared*/ false)
            {
                _creators["cast"]                = [](PsychobotAI* ai) -> Action* { return new CastSpellAction(ai); };
                _creators["cast melee"]          = [](PsychobotAI* ai) -> Action* { return new CastMeleeSpellAction(ai); };
                _creators["cast aura"]           = [](PsychobotAI* ai) -> Action* { return new CastAuraSpellAction(ai); };
                _creators["cast buff"]           = [](PsychobotAI* ai) -> Action* { return new CastBuffSpellAction(ai); };
                _creators["cast self"]           = [](PsychobotAI* ai) -> Action* { return new CastSelfSpellAction(ai); };
                _creators["cast heal"]           = [](PsychobotAI* ai) -> Action* { return new CastHealSpellAction(ai); };
                _creators["cast dispel"]         = [](PsychobotAI* ai) -> Action* { return new CastDispelSpellAction(ai); };
                _creators["cast melee aoe"]      = [](PsychobotAI* ai) -> Action* { return new CastMeleeAoeSpellAction(ai); };
                _creators["cast ranged debuff"]  = [](PsychobotAI* ai) -> Action* { return new CastRangedDebuffSpellAction(ai); };
                _creators["cast pet"]            = [](PsychobotAI* ai) -> Action* { return new CastPetSpellAction(ai); };
            }
        };
    }

    void RegisterGenericSpellActions(AiObjectContext* context)
    {
        if (context)
            context->AddActionContext(new GenericSpellActionContext());
    }
}
