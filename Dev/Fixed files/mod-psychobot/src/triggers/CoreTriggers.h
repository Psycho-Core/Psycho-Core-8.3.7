/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Core TRIGGERS (S4) - the conditions the generic + class strategies bind to
 *  actions. Each polls the core VALUES (S4) through the AiObjectContext and
 *  returns true when its condition holds; the engine then queues the trigger's
 *  bound NextAction handlers.
 *
 *  Health:   "low health", "medium health", "critical health"
 *  Combat:   "enemy is close" (in melee), "enemy out of melee", "has target",
 *            "no target", "target casting" (=interrupt window)
 *  Resource: "enough <power>" via thresholds (qualified)
 *  Aura:     "has aura"::id, "no aura"::id, "target has no aura"::id
 *  Spell:    "spell ready"::id
 *  Group:    "party member low health" (someone to heal)
 * ===========================================================================
 */

#ifndef PSYCHOBOT_TRIGGERS_CORETRIGGERS_H
#define PSYCHOBOT_TRIGGERS_CORETRIGGERS_H

#include "../engine/Trigger.h"
#include "../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Base for triggers that read the bot's AiObjectContext.
    class ContextTrigger : public Trigger
    {
    public:
        ContextTrigger(PsychobotAI* ai, std::string name) : Trigger(ai, std::move(name)) { }
    protected:
        AiObjectContext* Context() const;     // resolved via the bridge
    };

    // --- health triggers (bot self) ---------------------------------------
    class HealthBelowTrigger : public ContextTrigger
    {
    public:
        HealthBelowTrigger(PsychobotAI* ai, std::string name, float pct)
            : ContextTrigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // --- combat / target triggers -----------------------------------------
    class HasTargetTrigger : public ContextTrigger
    {
    public:
        HasTargetTrigger(PsychobotAI* ai) : ContextTrigger(ai, "has target") { }
        bool IsActive() override;
    };

    class NoTargetTrigger : public ContextTrigger
    {
    public:
        NoTargetTrigger(PsychobotAI* ai) : ContextTrigger(ai, "no target") { }
        bool IsActive() override;
    };

    class EnemyInMeleeTrigger : public ContextTrigger
    {
    public:
        EnemyInMeleeTrigger(PsychobotAI* ai) : ContextTrigger(ai, "enemy is close") { }
        bool IsActive() override;
    };

    class EnemyOutOfMeleeTrigger : public ContextTrigger
    {
    public:
        EnemyOutOfMeleeTrigger(PsychobotAI* ai) : ContextTrigger(ai, "enemy out of melee") { }
        bool IsActive() override;
    };

    class TargetCastingTrigger : public ContextTrigger
    {
    public:
        TargetCastingTrigger(PsychobotAI* ai) : ContextTrigger(ai, "target casting") { }
        bool IsActive() override;
    };

    // --- aura triggers (qualified by spell id) ----------------------------
    class HasAuraTrigger : public ContextTrigger, public Qualified
    {
    public:
        HasAuraTrigger(PsychobotAI* ai) : ContextTrigger(ai, "has aura") { }
        bool IsActive() override;
    };

    class NoAuraTrigger : public ContextTrigger, public Qualified
    {
    public:
        NoAuraTrigger(PsychobotAI* ai) : ContextTrigger(ai, "no aura") { }
        bool IsActive() override;
    };

    class TargetNoAuraTrigger : public ContextTrigger, public Qualified
    {
    public:
        TargetNoAuraTrigger(PsychobotAI* ai) : ContextTrigger(ai, "target has no aura") { }
        bool IsActive() override;
    };

    // --- spell-ready trigger (qualified by spell id) ----------------------
    class SpellReadyTrigger : public ContextTrigger, public Qualified
    {
    public:
        SpellReadyTrigger(PsychobotAI* ai) : ContextTrigger(ai, "spell ready") { }
        bool IsActive() override;
    };

    // --- group trigger ----------------------------------------------------
    class PartyMemberLowHealthTrigger : public ContextTrigger
    {
    public:
        PartyMemberLowHealthTrigger(PsychobotAI* ai) : ContextTrigger(ai, "party member low health") { }
        bool IsActive() override;
    };

    // --- healing-depth triggers (S21) -------------------------------------
    // A group member (or self) has a dispellable debuff -> cure it.
    class PartyMemberNeedsDispelTrigger : public ContextTrigger
    {
    public:
        PartyMemberNeedsDispelTrigger(PsychobotAI* ai) : ContextTrigger(ai, "party member needs dispel") { }
        bool IsActive() override;
    };

    // The bot's mana is below `pct` -> conserve (use efficient heals only).
    class LowManaTrigger : public ContextTrigger
    {
    public:
        LowManaTrigger(PsychobotAI* ai, float pct) : ContextTrigger(ai, "low mana"), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Register all core triggers into the bot's base trigger context.
    void RegisterCoreTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_TRIGGERS_CORETRIGGERS_H
