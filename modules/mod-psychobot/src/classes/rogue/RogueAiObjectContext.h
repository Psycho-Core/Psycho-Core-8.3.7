/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Rogue context bundle (S11). Layers Rogue triggers + spec strategies onto the
 *  bot's base AiObjectContext and maps spec index -> combat strategy name.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ROGUE_AIOBJECTCONTEXT_H
#define PSYCHOBOT_ROGUE_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Rogue
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Assassination, 1=Outlaw, 2=Subtlety. Falls back to "assassination".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_ROGUE_AIOBJECTCONTEXT_H
