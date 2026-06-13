/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Monk context bundle (S17 - NEW class). Layers Monk triggers + spec strategies
 *  onto the bot's base AiObjectContext and maps spec index -> strategy name.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_MONK_AIOBJECTCONTEXT_H
#define PSYCHOBOT_MONK_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Monk
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Brewmaster, 1=Mistweaver, 2=Windwalker. Falls back to "windwalker".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_MONK_AIOBJECTCONTEXT_H
