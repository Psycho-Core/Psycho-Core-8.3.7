/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Warlock context bundle (S15). Layers Warlock triggers + spec strategies onto
 *  the bot's base AiObjectContext and maps spec index -> combat strategy name.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_WARLOCK_AIOBJECTCONTEXT_H
#define PSYCHOBOT_WARLOCK_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Warlock
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Affliction, 1=Demonology, 2=Destruction. Falls back to "affliction".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_WARLOCK_AIOBJECTCONTEXT_H
