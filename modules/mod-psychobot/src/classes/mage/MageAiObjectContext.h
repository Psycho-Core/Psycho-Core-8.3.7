/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Mage context bundle (S14). Layers Mage triggers + spec strategies onto the
 *  bot's base AiObjectContext and maps spec index -> combat strategy name.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_MAGE_AIOBJECTCONTEXT_H
#define PSYCHOBOT_MAGE_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace Mage
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Arcane, 1=Fire, 2=Frost. Falls back to "frost".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_MAGE_AIOBJECTCONTEXT_H
