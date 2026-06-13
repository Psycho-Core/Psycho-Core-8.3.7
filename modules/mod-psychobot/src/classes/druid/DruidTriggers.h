/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Druid TRIGGERS (S16) - the 4-spec class with FORMS. On top of the generic
 *  vocabulary, the Druid adds:
 *    "astral power high"  - >= 80% Astral Power (Balance: spend Starsurge)
 *    "combo points full"  - >= 5 combo points (Feral: spend Ferocious Bite/Rip)
 *    "rage high"          - >= 70% Rage (Guardian: Maul / Ironfur)
 *    "moonkin form absent"- not in Moonkin Form (Balance form upkeep)
 *    "cat form absent"    - not in Cat Form (Feral form upkeep)
 *    "bear form absent"   - not in Bear Form (Guardian form upkeep)
 *    "moonfire absent" / "sunfire absent" - target lacks the DoT (Balance)
 *    "rake absent" / "rip absent"         - target lacks the bleed (Feral)
 *    "thrash absent"      - target lacks Thrash bleed (Guardian)
 *    "ironfur absent"     - self lacks Ironfur (Guardian mitigation)
 *  Astral Power / Rage as percents; combo points as a RAW count (max 5).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DRUID_TRIGGERS_H
#define PSYCHOBOT_DRUID_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // A power resource (by Powers id) >= threshold percent.
    class DruidPowerAboveTrigger : public Trigger
    {
    public:
        DruidPowerAboveTrigger(PsychobotAI* ai, std::string name, uint32 powerType, float pct)
            : Trigger(ai, std::move(name)), _powerType(powerType), _pct(pct) { }
        bool IsActive() override;
    private:
        uint32 _powerType;
        float  _pct;
    };

    // Combo points >= count (raw, POWER_COMBO_POINTS).
    class DruidComboPointsAboveTrigger : public Trigger
    {
    public:
        DruidComboPointsAboveTrigger(PsychobotAI* ai, std::string name, uint32 count)
            : Trigger(ai, std::move(name)), _count(count) { }
        bool IsActive() override;
    private:
        uint32 _count;
    };

    // Target MISSING a (named) DoT/bleed.
    class DruidTargetMissingAuraTrigger : public Trigger
    {
    public:
        DruidTargetMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self MISSING a (named) form/buff.
    class DruidSelfMissingAuraTrigger : public Trigger
    {
    public:
        DruidSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterDruidTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_DRUID_TRIGGERS_H
