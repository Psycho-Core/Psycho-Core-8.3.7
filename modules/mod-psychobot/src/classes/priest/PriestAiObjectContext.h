/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Priest context bundle (S12). Layers Priest triggers + spec strategies onto
 *  the bot's base AiObjectContext and maps spec index -> combat strategy name.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PRIEST_AIOBJECTCONTEXT_H
#define PSYCHOBOT_PRIEST_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Priest
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Discipline, 1=Holy, 2=Shadow. Falls back to "discipline".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_PRIEST_AIOBJECTCONTEXT_H
