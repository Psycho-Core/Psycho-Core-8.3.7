/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Mage TRIGGERS (S14). On top of the generic vocabulary, the Mage adds the
 *  proc-driven conditions the BfA 8.3 specs key off:
 *    "arcane charges high"  - >= 80% Arcane Charges (Arcane: dump w/ Barrage)
 *    "clearcasting"         - Clearcasting proc up (Arcane: free Arcane Missiles)
 *    "hot streak"           - Hot Streak proc up (Fire: instant Pyroblast)
 *    "brain freeze"         - Brain Freeze proc up (Frost: instant Flurry)
 *    "fingers of frost"     - Fingers of Frost proc up (Frost: shatter Ice Lance)
 *    "arcane intellect absent" - self lacks Arcane Intellect (buff upkeep)
 *  Arcane Charges read as a percent (POWER_ARCANE_CHARGES=16). Procs by NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_MAGE_TRIGGERS_H
#define PSYCHOBOT_MAGE_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Arcane Charges >= threshold percent (POWER_ARCANE_CHARGES = 16, max 4).
    class ArcaneChargesAboveTrigger : public Trigger
    {
    public:
        ArcaneChargesAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Self HAS a (named) proc aura up.
    class MageSelfHasAuraTrigger : public Trigger
    {
    public:
        MageSelfHasAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Target health <= threshold percent (Fire: Scorch execute).
    class MageTargetHealthBelowTrigger : public Trigger
    {
    public:
        MageTargetHealthBelowTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Self MISSING a (named) buff (Arcane Intellect).
    class MageSelfMissingAuraTrigger : public Trigger
    {
    public:
        MageSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterMageTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_MAGE_TRIGGERS_H
