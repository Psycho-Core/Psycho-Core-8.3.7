/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Hunter TRIGGERS (S10). On top of the generic engine vocabulary (target /
 *  range / health / aura-by-name / spell-ready-by-name), the Hunter adds:
 *    "focus high"       - >= 70% Focus (spend: Cobra Shot / Arcane Shot / Raptor)
 *    "focus enough"     - >= 40% Focus (builder/spender threshold)
 *    "target low health"- target <= 20% (Kill Shot execute)
 *    "no pet"           - the bot has no living pet (BM/Survival need one)
 *    "pet hurt"         - the pet is below 60% health (Mend Pet)
 *    "serpent sting absent" - target lacks Serpent Sting (MM/Survival DoT)
 *  All read core values by NAME (or pet state via ServerFacade) -> DB2-agnostic.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_HUNTER_TRIGGERS_H
#define PSYCHOBOT_HUNTER_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Focus >= threshold percent (POWER_FOCUS = 2).
    class FocusAboveTrigger : public Trigger
    {
    public:
        FocusAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target health <= threshold percent (Kill Shot execute window).
    class HunterTargetHealthBelowTrigger : public Trigger
    {
    public:
        HunterTargetHealthBelowTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // The bot currently has NO living pet (needs to summon/revive one).
    class NoPetTrigger : public Trigger
    {
    public:
        NoPetTrigger(PsychobotAI* ai) : Trigger(ai, "no pet") { }
        bool IsActive() override;
    };

    // The bot's pet is alive but hurt below `pct` health (Mend Pet).
    class PetHurtTrigger : public Trigger
    {
    public:
        PetHurtTrigger(PsychobotAI* ai, float pct) : Trigger(ai, "pet hurt"), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target MISSING a (named) debuff (Serpent Sting upkeep).
    class HunterTargetMissingAuraTrigger : public Trigger
    {
    public:
        HunterTargetMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterHunterTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_HUNTER_TRIGGERS_H
