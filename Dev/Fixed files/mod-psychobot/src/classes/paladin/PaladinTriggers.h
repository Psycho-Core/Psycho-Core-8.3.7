/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Paladin TRIGGERS (S9). On top of the generic engine vocabulary (target /
 *  range / health / aura-by-name / spell-ready-by-name / party member low
 *  health), the Paladin adds:
 *    "holy power high"      - >= 3 Holy Power (spend: Templar's Verdict / SotR /
 *                             Word of Glory / Light of Dawn). HP is 0..5 so the
 *                             threshold is expressed as a percent (>=60% ~ 3).
 *    "target low health"    - target <= 20% (Hammer of Wrath execute).
 *    "devotion aura absent" - not under Devotion Aura (upkeep).
 *    "shield of righteous absent" - SotR buff down (Prot mitigation).
 *  All read core values by NAME -> DB2-agnostic.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PALADIN_TRIGGERS_H
#define PSYCHOBOT_PALADIN_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Holy Power >= threshold percent (POWER_HOLY_POWER = 9; max 5).
    class HolyPowerAboveTrigger : public Trigger
    {
    public:
        HolyPowerAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target health <= threshold percent (Hammer of Wrath execute window).
    class PaladinTargetHealthBelowTrigger : public Trigger
    {
    public:
        PaladinTargetHealthBelowTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Self MISSING a (named) aura/blessing (upkeep).
    class PaladinSelfMissingAuraTrigger : public Trigger
    {
    public:
        PaladinSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterPaladinTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_PALADIN_TRIGGERS_H
