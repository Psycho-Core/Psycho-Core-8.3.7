/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Monk TRIGGERS (S17 - NEW class, no cmangos reference; authored from BfA 8.3
 *  research). On top of the generic vocabulary, the Monk adds:
 *    "chi high"           - >= 80% Chi (Windwalker: spend Rising Sun/Blackout)
 *    "chi enough"         - >= 2 Chi (Windwalker: a spender is affordable)
 *    "energy high"        - >= 70% Energy (don't cap; spend a builder)
 *    "ironskin brew absent" - self lacks Ironskin Brew (Brewmaster mitigation)
 *    "shuffle absent"     - self lacks Shuffle (Brewmaster stagger buff)
 *  Chi read as a RAW count (POWER_CHI=12, max 5/6); Energy as a percent.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_MONK_TRIGGERS_H
#define PSYCHOBOT_MONK_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Chi >= count (raw, POWER_CHI).
    class ChiAboveTrigger : public Trigger
    {
    public:
        ChiAboveTrigger(PsychobotAI* ai, std::string name, uint32 count)
            : Trigger(ai, std::move(name)), _count(count) { }
        bool IsActive() override;
    private:
        uint32 _count;
    };

    // Energy >= threshold percent (POWER_ENERGY = 3).
    class MonkEnergyAboveTrigger : public Trigger
    {
    public:
        MonkEnergyAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target health <= threshold percent (Touch of Death execute).
    class MonkTargetHealthBelowTrigger : public Trigger
    {
    public:
        MonkTargetHealthBelowTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Self MISSING a (named) buff (Ironskin Brew / Shuffle).
    class MonkSelfMissingAuraTrigger : public Trigger
    {
    public:
        MonkSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterMonkTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_MONK_TRIGGERS_H
