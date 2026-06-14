/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  The CastSpellAction FAMILY (S5 - the KEYSTONE).
 *  -----------------------------------------------
 *  Almost every per-class behaviour is "cast spell X under condition Y". Rather
 *  than a C++ class per spell, a class folder REGISTERS named instances of this
 *  small family (qualified by spell name, e.g. "cast::frostbolt"). The family
 *  resolves the spell from its name to the id the bot actually knows, picks the
 *  right target, checks range/resources, and casts via the ServerFacade seam.
 *
 *    CastSpellAction          - ranged single-target cast on "current target"
 *    CastMeleeSpellAction     - requires melee range; prereq = "reach melee"
 *    CastAuraSpellAction      - only casts if the target lacks the aura
 *    CastBuffSpellAction      - casts on self if self lacks the buff
 *    CastMeleeAoeSpellAction  - melee AoE (threat = AOE)
 *    CastRangedDebuffSpellAction - ranged; only if target lacks the debuff
 *    CastPetSpellAction       - pet ability (no GCD/range gate on the bot)
 *
 *  Class strategies register these by qualifier; the qualifier IS the spell
 *  name, so wiring reads like the reference engine ("cast::mortal strike").
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ACTIONS_GENERICSPELLACTIONS_H
#define PSYCHOBOT_ACTIONS_GENERICSPELLACTIONS_H

#include "../engine/Action.h"
#include "../engine/NamedObjectContext.h"
#include "../engine/NextAction.h"

class Unit;

namespace psychobot
{
    // ----------------------------------------------------------------------
    // CastSpellAction - the base: cast `qualifier` (a spell name) at the
    // current target if known, useful and possible.
    // ----------------------------------------------------------------------
    class CastSpellAction : public Action, public Qualified
    {
    public:
        CastSpellAction(PsychobotAI* ai, std::string name = "cast")
            : Action(ai, std::move(name)) { }

        bool Execute(Event const& event) override;
        bool IsPossible() override;
        bool IsUseful() override;
        ActionThreatType GetThreatType() override { return ActionThreatType::ACTION_THREAT_SINGLE; }

        Unit* GetTarget() override;                                  // current target
        NextAction** GetPrerequisites() override;                   // reach range first

    protected:
        // Resolve the qualifier spell name to the id the bot knows (0 if not).
        uint32 ResolveSpell();
        // Range the bot must be within to cast (default = ranged spell range).
        virtual float GetCastRange();
        // The prerequisite that closes distance ("reach spell" / "reach melee").
        virtual std::string GetReachActionName() { return "reach spell"; }
    };

    // ----------------------------------------------------------------------
    // CastMeleeSpellAction - must be in melee range; reaches melee first.
    // ----------------------------------------------------------------------
    class CastMeleeSpellAction : public CastSpellAction
    {
    public:
        CastMeleeSpellAction(PsychobotAI* ai, std::string name = "cast melee")
            : CastSpellAction(ai, std::move(name)) { }
    protected:
        float GetCastRange() override;                              // melee reach
        std::string GetReachActionName() override { return "reach melee"; }
    };

    // ----------------------------------------------------------------------
    // CastAuraSpellAction - only cast if the TARGET does not already have the
    // aura (avoids re-applying a debuff every GCD).
    // ----------------------------------------------------------------------
    class CastAuraSpellAction : public CastSpellAction
    {
    public:
        CastAuraSpellAction(PsychobotAI* ai, std::string name = "cast aura")
            : CastSpellAction(ai, std::move(name)) { }
        bool IsUseful() override;
    };

    // ----------------------------------------------------------------------
    // CastBuffSpellAction - cast on SELF if self lacks the buff.
    // ----------------------------------------------------------------------
    class CastBuffSpellAction : public CastSpellAction
    {
    public:
        CastBuffSpellAction(PsychobotAI* ai, std::string name = "cast buff")
            : CastSpellAction(ai, std::move(name)) { }
        bool  IsUseful() override;
        Unit* GetTarget() override;                                 // self
    protected:
        float GetCastRange() override { return 0.0f; }             // self-cast
        std::string GetReachActionName() override { return ""; }   // no reach
    };

    // ----------------------------------------------------------------------
    // CastSelfSpellAction - self-cast instant with NO target/aura gate (used for
    // summons / cooldowns like Raise Dead, Horn of Winter, Army of the Dead).
    // Only gated by spell-ready, so it fires once then sits on cooldown.
    // ----------------------------------------------------------------------
    class CastSelfSpellAction : public CastSpellAction
    {
    public:
        CastSelfSpellAction(PsychobotAI* ai, std::string name = "cast self")
            : CastSpellAction(ai, std::move(name)) { }
        bool  IsUseful() override;
        Unit* GetTarget() override;                                 // self
    protected:
        float GetCastRange() override { return 0.0f; }
        std::string GetReachActionName() override { return ""; }
    };

    // ----------------------------------------------------------------------
    // CastHealSpellAction - cast a heal on the "party member to heal" value
    // (most-injured group member, or self when solo). Ranged friendly cast.
    // Used by healer specs (S9 Paladin Holy onward).
    // ----------------------------------------------------------------------
    class CastHealSpellAction : public CastSpellAction
    {
    public:
        CastHealSpellAction(PsychobotAI* ai, std::string name = "cast heal")
            : CastSpellAction(ai, std::move(name)) { }
        bool  IsUseful() override;
        Unit* GetTarget() override;                                 // heal target
    protected:
        float GetCastRange() override { return 40.0f; }
        std::string GetReachActionName() override { return ""; }   // no chase to heal
    };

    // ----------------------------------------------------------------------
    // CastDispelSpellAction - cast a cure/dispel on the "party member to dispel"
    // value (a group member carrying a removable debuff). Ranged friendly cast.
    // Used by healer dispel wiring (S21).
    // ----------------------------------------------------------------------
    class CastDispelSpellAction : public CastSpellAction
    {
    public:
        CastDispelSpellAction(PsychobotAI* ai, std::string name = "cast dispel")
            : CastSpellAction(ai, std::move(name)) { }
        bool  IsUseful() override;
        Unit* GetTarget() override;                                 // dispel target
    protected:
        float GetCastRange() override { return 40.0f; }
        std::string GetReachActionName() override { return ""; }
    };

    // ----------------------------------------------------------------------
    // CastMeleeAoeSpellAction - melee AoE (e.g. cleave/whirlwind).
    // ----------------------------------------------------------------------
    class CastMeleeAoeSpellAction : public CastMeleeSpellAction
    {
    public:
        CastMeleeAoeSpellAction(PsychobotAI* ai, std::string name = "cast melee aoe")
            : CastMeleeSpellAction(ai, std::move(name)) { }
        ActionThreatType GetThreatType() override { return ActionThreatType::ACTION_THREAT_AOE; }
    };

    // ----------------------------------------------------------------------
    // CastRangedDebuffSpellAction - ranged; only if target lacks the debuff.
    // ----------------------------------------------------------------------
    class CastRangedDebuffSpellAction : public CastAuraSpellAction
    {
    public:
        CastRangedDebuffSpellAction(PsychobotAI* ai, std::string name = "cast ranged debuff")
            : CastAuraSpellAction(ai, std::move(name)) { }
    };

    // ----------------------------------------------------------------------
    // CastPetSpellAction - a pet ability (skip the bot's range/GCD gates).
    // ----------------------------------------------------------------------
    class CastPetSpellAction : public CastSpellAction
    {
    public:
        CastPetSpellAction(PsychobotAI* ai, std::string name = "cast pet")
            : CastSpellAction(ai, std::move(name)) { }
        NextAction** GetPrerequisites() override { return nullptr; }   // no reach
    protected:
        float GetCastRange() override { return 30.0f; }
    };

    // Register the whole family into the bot's base action context.
    class AiObjectContext;
    void RegisterGenericSpellActions(AiObjectContext* context);
}

#endif // PSYCHOBOT_ACTIONS_GENERICSPELLACTIONS_H
