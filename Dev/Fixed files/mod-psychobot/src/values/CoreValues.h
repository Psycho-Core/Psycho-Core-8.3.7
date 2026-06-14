/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Core VALUES (S4) - the shared world-knowledge the generic + class strategies
 *  read every tick. Each value is a CalculatedValue<T> registered by name in
 *  the base AiObjectContext (see RegisterCoreValues). Triggers and actions then
 *  query them via context->GetValue<T>("name").
 *
 *  Target values:   "current target", "self target", "master target"
 *  Resource values: "health", "mana", "rage", "energy", "runic power",
 *                   "combo points", "chi", "fury", "pain", "holy power",
 *                   "soul shards", "energy"  (percent-based health/power)
 *  State values:    "has aura"::spellId, "spell ready"::spellId,
 *                   "distance to target", "in combat"
 *  Group values:    "party member to heal" (most-injured group member)
 *
 *  All resource percents are 0..100. The "::qualifier" form (e.g.
 *  "has aura::12345") is parsed by NamedObjectFactory and applied via Qualified.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_VALUES_COREVALUES_H
#define PSYCHOBOT_VALUES_COREVALUES_H

#include "../engine/Value.h"
#include "../engine/AiObject.h"
#include "../engine/NamedObjectContext.h"

class Unit;

namespace psychobot
{
    class AiObjectContext;

    // ----------------------------------------------------------------------
    // Target values (Value<Unit*>)
    // ----------------------------------------------------------------------
    class CurrentTargetValue : public CalculatedValue<Unit*>
    {
    public:
        CurrentTargetValue(PsychobotAI* ai)
            : CalculatedValue<Unit*>(ai, "current target", 0) { }
    protected:
        Unit* Calculate() override;
    };

    class SelfTargetValue : public CalculatedValue<Unit*>
    {
    public:
        SelfTargetValue(PsychobotAI* ai)
            : CalculatedValue<Unit*>(ai, "self target", 1) { }
    protected:
        Unit* Calculate() override;     // the bot itself, as a Unit*
    };

    class MasterTargetValue : public CalculatedValue<Unit*>
    {
    public:
        MasterTargetValue(PsychobotAI* ai)
            : CalculatedValue<Unit*>(ai, "master target", 0) { }
    protected:
        Unit* Calculate() override;     // master's selected unit
    };

    // ----------------------------------------------------------------------
    // Health / resource percent values (Value<float>, 0..100)
    // ----------------------------------------------------------------------
    class HealthValue : public CalculatedValue<float>
    {
    public:
        HealthValue(PsychobotAI* ai)
            : CalculatedValue<float>(ai, "health", 0) { }
    protected:
        float Calculate() override;
    };

    // Generic power-percent value qualified by Powers id (e.g. "power::3").
    class PowerPctValue : public CalculatedValue<float>, public Qualified
    {
    public:
        PowerPctValue(PsychobotAI* ai)
            : CalculatedValue<float>(ai, "power", 0) { }
    protected:
        float Calculate() override;     // qualifier = Powers enum value
    };

    // ----------------------------------------------------------------------
    // State values
    // ----------------------------------------------------------------------
    class DistanceToTargetValue : public CalculatedValue<float>
    {
    public:
        DistanceToTargetValue(PsychobotAI* ai)
            : CalculatedValue<float>(ai, "distance to target", 0) { }
    protected:
        float Calculate() override;
    };

    class InCombatValue : public CalculatedValue<bool>
    {
    public:
        InCombatValue(PsychobotAI* ai)
            : CalculatedValue<bool>(ai, "in combat", 0) { }
    protected:
        bool Calculate() override;
    };

    // "has aura::spellId" - bool, qualified by spell id.
    class HasAuraValue : public CalculatedValue<bool>, public Qualified
    {
    public:
        HasAuraValue(PsychobotAI* ai)
            : CalculatedValue<bool>(ai, "has aura", 0) { }
    protected:
        bool Calculate() override;      // checks aura on the bot
    };

    // "target has aura::spellId" - bool, qualified by spell id (checks target).
    class TargetHasAuraValue : public CalculatedValue<bool>, public Qualified
    {
    public:
        TargetHasAuraValue(PsychobotAI* ai)
            : CalculatedValue<bool>(ai, "target has aura", 0) { }
    protected:
        bool Calculate() override;
    };

    // "spell ready::spellId" - bool, qualified by spell id (known + off CD).
    class SpellReadyValue : public CalculatedValue<bool>, public Qualified
    {
    public:
        SpellReadyValue(PsychobotAI* ai)
            : CalculatedValue<bool>(ai, "spell ready", 0) { }
    protected:
        bool Calculate() override;
    };

    // ----------------------------------------------------------------------
    // Group values
    // ----------------------------------------------------------------------
    class PartyMemberToHealValue : public CalculatedValue<Unit*>
    {
    public:
        PartyMemberToHealValue(PsychobotAI* ai)
            : CalculatedValue<Unit*>(ai, "party member to heal", 1) { }
    protected:
        Unit* Calculate() override;
    };

    // S21: a group member (or self) carrying a dispellable debuff (any type),
    // or nullptr. Used by healer cure wiring.
    class PartyMemberToDispelValue : public CalculatedValue<Unit*>
    {
    public:
        PartyMemberToDispelValue(PsychobotAI* ai)
            : CalculatedValue<Unit*>(ai, "party member to dispel", 1) { }
    protected:
        Unit* Calculate() override;
    };

    // Register all core values into the bot's base value context.
    void RegisterCoreValues(AiObjectContext* context);
}

#endif // PSYCHOBOT_VALUES_COREVALUES_H
