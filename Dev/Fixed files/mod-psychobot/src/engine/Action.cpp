/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "Action.h"
#include "PsychobotAIFwd.h"
#include "Player.h"   // Player derives from Unit -> safe implicit upcast

namespace psychobot
{
    // Default target = the action's owning bot (a Player IS-A Unit). Targeted
    // combat actions override GetTarget() to read the "current target" value.
    Unit* Action::GetTarget()
    {
        Player* bot = GetBot();
        return bot;   // implicit Player* -> Unit* upcast
    }
}
