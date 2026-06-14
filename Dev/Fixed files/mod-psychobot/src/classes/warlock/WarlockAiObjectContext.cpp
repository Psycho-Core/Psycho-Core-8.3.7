/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "WarlockAiObjectContext.h"
#include "WarlockTriggers.h"
#include "WarlockStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Warlock
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterWarlockTriggers(context);
            RegisterWarlockStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "affliction";  // DPS
                case 1: return "demonology";  // DPS
                case 2: return "destruction"; // DPS
                default: return "affliction";
            }
        }
    }
}
