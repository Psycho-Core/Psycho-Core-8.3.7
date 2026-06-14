/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Paladin context bundle (S9). Layers Paladin triggers + spec strategies onto
 *  the bot's base AiObjectContext and maps spec index -> combat strategy name.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PALADIN_AIOBJECTCONTEXT_H
#define PSYCHOBOT_PALADIN_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Paladin
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Holy, 1=Protection, 2=Retribution. Falls back to "retribution".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_PALADIN_AIOBJECTCONTEXT_H
