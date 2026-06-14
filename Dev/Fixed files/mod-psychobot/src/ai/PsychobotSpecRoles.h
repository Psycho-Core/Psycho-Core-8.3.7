/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotSpecRoles - the stable class+spec -> role (tank/heal/dps) table the
 *  managers and engine consume. (Extracted from the old data-driven ClassAI so
 *  it survives the move to the real engine; per-class combat is now done by the
 *  engine's class strategies, but the role mapping is still needed for grouping,
 *  LFG roles, and picking which generic combat strategy a bot runs.)
 * ===========================================================================
 */

#ifndef PSYCHOBOT_SPEC_ROLES_H
#define PSYCHOBOT_SPEC_ROLES_H

#include "Define.h"
#include <string>

class Player;

namespace psychobot
{
    enum class SpecRole : uint8
    {
        Dps    = 0,
        Tank   = 1,
        Healer = 2
    };

    namespace SpecRoles
    {
        // Role for a class' spec (by ChrSpecialization OrderIndex 0..n).
        // Falls back to Dps for unknown class/spec combinations.
        SpecRole GetRole(uint8 classId, uint8 specIndex);

        // Human-readable spec name (e.g. "Frost", "Havoc"), "" if unknown.
        std::string GetSpecName(uint8 classId, uint8 specIndex);

        // Role of a live bot derived from its current primary specialization.
        SpecRole GetBotRole(Player* bot);

        // The bot's spec OrderIndex (0..n), or 0 if none/unknown.
        uint8 GetBotSpecIndex(Player* bot);

        // Whether a class' default playstyle is melee (picks the generic combat
        // strategy at S6). Refined by per-class strategies later.
        bool IsMeleeClass(uint8 classId);
    }
}

#endif // PSYCHOBOT_SPEC_ROLES_H
