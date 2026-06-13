/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Demon Hunter TRIGGERS (S18 - NEW class, no cmangos reference; authored from
 *  BfA 8.3 research). The final class - only TWO specs. On top of the generic
 *  vocabulary, the DH adds:
 *    "fury high"          - >= 70% Fury (Havoc: spend Chaos Strike / Blade Dance)
 *    "pain high"          - >= 70% Pain (Vengeance: spend Soul Cleave)
 *    "soul fragments high"- has Soul Fragments (Vengeance: Spirit Bomb / Cleave)
 *    "immolation aura absent" - self lacks Immolation Aura (gen + AoE upkeep)
 *    "demon spikes absent"    - self lacks Demon Spikes (Vengeance mitigation)
 *  Fury/Pain read as percents (POWER_FURY=17 / POWER_PAIN=18). Fragments by aura.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DEMONHUNTER_TRIGGERS_H
#define PSYCHOBOT_DEMONHUNTER_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // A power resource (by Powers id) >= threshold percent.
    class DHPowerAboveTrigger : public Trigger
    {
    public:
        DHPowerAboveTrigger(PsychobotAI* ai, std::string name, uint32 powerType, float pct)
            : Trigger(ai, std::move(name)), _powerType(powerType), _pct(pct) { }
        bool IsActive() override;
    private:
        uint32 _powerType;
        float  _pct;
    };

    // Self HAS a (named) aura up (Soul Fragments).
    class DHSelfHasAuraTrigger : public Trigger
    {
    public:
        DHSelfHasAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self MISSING a (named) buff (Immolation Aura / Demon Spikes).
    class DHSelfMissingAuraTrigger : public Trigger
    {
    public:
        DHSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterDemonHunterTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_DEMONHUNTER_TRIGGERS_H
