/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Priest TRIGGERS (S12). On top of the generic vocabulary (target / range /
 *  health / aura-by-name / spell-ready-by-name / party member low health), the
 *  Priest adds:
 *    "insanity high"          - >= 80% Insanity (Shadow: spend Devouring Plague)
 *    "shadow word pain absent"- target lacks Shadow Word: Pain (Shadow DoT)
 *    "vampiric touch absent"  - target lacks Vampiric Touch (Shadow DoT)
 *    "atonement low"          - the heal target lacks Atonement (Disc apply PWS)
 *    "shadowform absent"      - not in Shadowform (Shadow upkeep)
 *  Insanity read as a percent (POWER_INSANITY=13). All else by NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PRIEST_TRIGGERS_H
#define PSYCHOBOT_PRIEST_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Insanity >= threshold percent (POWER_INSANITY = 13).
    class InsanityAboveTrigger : public Trigger
    {
    public:
        InsanityAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target health <= threshold percent (Shadow Word: Death execute window).
    class PriestTargetHealthBelowTrigger : public Trigger
    {
    public:
        PriestTargetHealthBelowTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target MISSING a (named) DoT/debuff.
    class PriestTargetMissingAuraTrigger : public Trigger
    {
    public:
        PriestTargetMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self MISSING a (named) form/buff (Shadowform).
    class PriestSelfMissingAuraTrigger : public Trigger
    {
    public:
        PriestSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // The heal target (party member to heal) is MISSING Atonement (Disc).
    class HealTargetMissingAuraTrigger : public Trigger
    {
    public:
        HealTargetMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterPriestTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_PRIEST_TRIGGERS_H
