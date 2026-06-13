/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Warrior TRIGGERS (S8). The generic engine already provides target / range /
 *  health / aura(by name) / spell-ready(by name) triggers (S4); the Warrior
 *  only adds class-specific conditions the BfA 8.3 rotations key off:
 *    "rage high"        - >= 80% Rage (spend it: Rampage / Slam)
 *    "rage enough"      - >= 30% Rage (rage-spender threshold)
 *    "target low health"- target <= 20% (Execute window)
 *    "overpower ready"  - Overpower proc / charge available (Arms)
 *    "sudden death"     - Sudden Death proc (free Execute)
 *    "enrage absent"    - not Enraged (Fury: Rampage to refresh)
 *    "battle stance absent"   - not in Battle Stance (Arms/Fury upkeep)
 *    "defensive stance absent"- not in Defensive Stance (Protection upkeep)
 *    "shield block absent"    - Shield Block down (Protection mitigation)
 *  All read core values by NAME -> DB2-agnostic.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_WARRIOR_TRIGGERS_H
#define PSYCHOBOT_WARRIOR_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Rage >= threshold (percent). POWER_RAGE = 1.
    class RageAboveTrigger : public Trigger
    {
    public:
        RageAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target health <= threshold (percent) -> Execute window.
    class TargetHealthBelowTrigger : public Trigger
    {
    public:
        TargetHealthBelowTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Self has a (named) proc/buff up.
    class WarriorSelfHasAuraTrigger : public Trigger
    {
    public:
        WarriorSelfHasAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self is MISSING a (named) buff/stance.
    class WarriorSelfMissingAuraTrigger : public Trigger
    {
    public:
        WarriorSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterWarriorTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_WARRIOR_TRIGGERS_H
