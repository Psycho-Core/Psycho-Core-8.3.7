/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "DKAiObjectContext.h"
#include "DKTriggers.h"
#include "DKStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace DeathKnight
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterDeathKnightTriggers(context);
            RegisterDeathKnightStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "blood";   // Tank
                case 1: return "frost";   // DPS
                case 2: return "unholy";  // DPS
                default: return "frost";
            }
        }
    }
}
