/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Demon Hunter context bundle (S18 - NEW class, the final one). Layers DH
 *  triggers + spec strategies onto the base context. DH is the only TWO-spec
 *  class (Havoc dps / Vengeance tank).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DEMONHUNTER_AIOBJECTCONTEXT_H
#define PSYCHOBOT_DEMONHUNTER_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace DemonHunter
    {
        void RegisterContext(AiObjectContext* context);

        // 0=Havoc, 1=Vengeance. Falls back to "havoc".
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_DEMONHUNTER_AIOBJECTCONTEXT_H
