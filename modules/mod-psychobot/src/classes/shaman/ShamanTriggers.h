/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Shaman TRIGGERS (S13). On top of the generic vocabulary, the Shaman adds:
 *    "maelstrom high"        - >= 80% Maelstrom (Elemental: spend Earth Shock)
 *    "maelstrom weapon high" - Maelstrom Weapon stacks up (Enh: cast Lightning
 *                              Bolt instantly) - checked via the named aura
 *    "flame shock absent"    - target lacks Flame Shock (Ele/Enh DoT upkeep)
 *    "windfury weapon absent"- main-hand lacks Windfury Weapon imbue (Enh)
 *    "flametongue weapon absent" - off-hand lacks Flametongue imbue (Enh)
 *    "lightning shield absent"   - self lacks Lightning Shield (Ele upkeep)
 *  Maelstrom read as a percent (POWER_MAELSTROM=11). All else by NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_SHAMAN_TRIGGERS_H
#define PSYCHOBOT_SHAMAN_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Maelstrom >= threshold percent (POWER_MAELSTROM = 11).
    class MaelstromAboveTrigger : public Trigger
    {
    public:
        MaelstromAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target MISSING a (named) DoT/debuff (Flame Shock).
    class ShamanTargetMissingAuraTrigger : public Trigger
    {
    public:
        ShamanTargetMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self HAS a (named) buff/proc (Maelstrom Weapon stacks).
    class ShamanSelfHasAuraTrigger : public Trigger
    {
    public:
        ShamanSelfHasAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self MISSING a (named) buff/imbue/shield.
    class ShamanSelfMissingAuraTrigger : public Trigger
    {
    public:
        ShamanSelfMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    void RegisterShamanTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_SHAMAN_TRIGGERS_H
