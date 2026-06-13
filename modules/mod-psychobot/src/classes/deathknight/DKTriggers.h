/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Death Knight TRIGGERS (S7). The generic engine already gives us target /
 *  range / health / aura(by name) / spell-ready(by name) triggers (S4), so the
 *  DK only needs a handful of class-specific conditions the BfA 8.3 rotations
 *  key off:
 *    "runic power high"  - >= 80% Runic Power (spend it: Frost Strike/Death Coil)
 *    "runic power enough"- >= 35% (the rune-power spender threshold)
 *    "frost fever absent"- target lacks Frost Fever (Frost: Howling Blast/Icy T.)
 *    "blood plague absent"-target lacks Blood Plague (Blood: Blood Boil)
 *    "virulent plague absent"-target lacks Virulent Plague (Unholy: Outbreak)
 *    "killing machine"   - Killing Machine proc up (Frost: free Obliterate)
 *    "rime"              - Rime proc up (Frost: free Howling Blast)
 *    "sudden doom"       - Sudden Doom proc up (Unholy: free Death Coil)
 *    "bone shield low"   - Bone Shield < 3 charges (Blood: Marrowrend)
 *  These read the core values (has aura by NAME / power pct) under the hood, so
 *  they're DB2-agnostic (no hardcoded 8.3 spell ids).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DK_TRIGGERS_H
#define PSYCHOBOT_DK_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Runic Power >= threshold (percent). POWER_RUNIC_POWER = 6.
    class RunicPowerAboveTrigger : public Trigger
    {
    public:
        RunicPowerAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target is missing a (named) disease/aura -> time to (re)apply it.
    class TargetMissingAuraTrigger : public Trigger
    {
    public:
        TargetMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self has a (named) proc aura up -> use the empowered ability.
    class SelfHasAuraTrigger : public Trigger
    {
    public:
        SelfHasAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self is MISSING a (named) maintenance buff (Bone Shield / presence-like).
    class SelfMissingAuraTrigger : public Trigger
    {
    public:
        SelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterDeathKnightTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_DK_TRIGGERS_H
