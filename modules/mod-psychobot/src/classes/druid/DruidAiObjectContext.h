/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Druid context bundle (S16). Layers Druid triggers + spec strategies onto the
 *  bot's base AiObjectContext and maps spec index -> combat strategy name.
 *  Druid is the only FOUR-spec class (Balance/Feral/Guardian/Restoration).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DRUID_AIOBJECTCONTEXT_H
#define PSYCHOBOT_DRUID_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Druid
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Balance, 1=Feral, 2=Guardian, 3=Restoration. Falls back to "balance".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_DRUID_AIOBJECTCONTEXT_H
