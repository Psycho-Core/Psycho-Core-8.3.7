/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Hunter context bundle (S10). Layers Hunter triggers + spec strategies onto
 *  the bot's base AiObjectContext and maps spec index -> combat strategy name.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_HUNTER_AIOBJECTCONTEXT_H
#define PSYCHOBOT_HUNTER_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Hunter
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Beast Mastery, 1=Marksmanship, 2=Survival. Falls back to "beast mastery".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_HUNTER_AIOBJECTCONTEXT_H
