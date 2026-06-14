/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "AiObject.h"
#include "PsychobotAIFwd.h"

namespace psychobot
{
    Player* AiObject::GetBot() const
    {
        return _ai ? PsychobotAIBridge::GetBot(_ai) : nullptr;
    }

    Player* AiObject::GetMaster() const
    {
        return _ai ? PsychobotAIBridge::GetMaster(_ai) : nullptr;
    }
}
