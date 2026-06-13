/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Warrior context bundle (S8). Layers Warrior triggers + spec strategies onto
 *  a bot's base AiObjectContext and maps spec index -> combat strategy name.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_WARRIOR_AIOBJECTCONTEXT_H
#define PSYCHOBOT_WARRIOR_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Warrior
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Arms, 1=Fury, 2=Protection. Falls back to "arms".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_WARRIOR_AIOBJECTCONTEXT_H
