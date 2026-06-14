/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Rogue TRIGGERS (S11). On top of the generic engine vocabulary, the Rogue
 *  adds combo-point / energy / stealth conditions the BfA 8.3 specs key off:
 *    "combo points full"  - >= 5 combo points (spend: Envenom/Eviscerate/etc.)
 *    "combo points low"   - <= 1 combo point (build more before finishing)
 *    "energy high"        - >= 70% Energy (don't cap; spend a builder)
 *    "stealthed"          - Stealth/Shadow Dance active (use openers)
 *    "not stealthed"      - no Stealth (use Stealth out of combat)
 *    "target low health"  - target <= 35% (Assassination/Sub execute pressure)
 *    "rupture absent"     - target lacks Rupture (Assassination bleed)
 *    "garrote absent"     - target lacks Garrote (Assassination bleed)
 *    "slice and dice absent" - self lacks Slice and Dice (Outlaw upkeep)
 *  Combo points read as a RAW count (POWER_COMBO_POINTS, max 5).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ROGUE_TRIGGERS_H
#define PSYCHOBOT_ROGUE_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Combo points >= `count` (raw, POWER_COMBO_POINTS).
    class ComboPointsAboveTrigger : public Trigger
    {
    public:
        ComboPointsAboveTrigger(PsychobotAI* ai, std::string name, uint32 count)
            : Trigger(ai, std::move(name)), _count(count) { }
        bool IsActive() override;
    private:
        uint32 _count;
    };

    // Combo points <= `count` (raw).
    class ComboPointsBelowTrigger : public Trigger
    {
    public:
        ComboPointsBelowTrigger(PsychobotAI* ai, std::string name, uint32 count)
            : Trigger(ai, std::move(name)), _count(count) { }
        bool IsActive() override;
    private:
        uint32 _count;
    };

    // Energy >= threshold percent (POWER_ENERGY = 3).
    class EnergyAboveTrigger : public Trigger
    {
    public:
        EnergyAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Stealth / Shadow Dance active (or not), checked via a named aura.
    class StealthedTrigger : public Trigger
    {
    public:
        StealthedTrigger(PsychobotAI* ai, std::string name, bool wantStealthed)
            : Trigger(ai, std::move(name)), _want(wantStealthed) { }
        bool IsActive() override;
    private:
        bool _want;
    };

    // Target health <= threshold percent.
    class RogueTargetHealthBelowTrigger : public Trigger
    {
    public:
        RogueTargetHealthBelowTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target MISSING a (named) bleed/debuff.
    class RogueTargetMissingAuraTrigger : public Trigger
    {
    public:
        RogueTargetMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self MISSING a (named) buff (Slice and Dice).
    class RogueSelfMissingAuraTrigger : public Trigger
    {
    public:
        RogueSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterRogueTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_ROGUE_TRIGGERS_H
