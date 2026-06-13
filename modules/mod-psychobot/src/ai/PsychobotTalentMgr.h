/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotTalentMgr - BfA 8.3 talent application. Generic for ALL classes:
 *  picks/activates a specialization, then learns one talent per unlocked tier
 *  (7 rows x 3 columns). Pure DB2-driven - no per-class hardcoding.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_TALENT_MGR_H
#define PSYCHOBOT_TALENT_MGR_H

#include "Define.h"

class Player;

namespace psychobot
{
    namespace TalentMgr
    {
        // Ensure the bot has a valid primary specialization for its class.
        // If specIndex < 0, uses the class default. Returns the chosen specId.
        uint32 EnsureSpecialization(Player* bot, int32 specIndex = -1);

        // Learn a talent for each unlocked tier (column choice = preferColumn,
        // clamped to a valid talent at that tier/spec). Safe to call repeatedly.
        void ApplyTalents(Player* bot, uint8 preferColumn = 0);

        // Convenience: ensure spec + apply talents in one call.
        void SetupSpec(Player* bot, int32 specIndex = -1, uint8 preferColumn = 0);
    }
}

#endif // PSYCHOBOT_TALENT_MGR_H
