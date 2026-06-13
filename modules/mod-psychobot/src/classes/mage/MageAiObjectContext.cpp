/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "MageAiObjectContext.h"
#include "MageTriggers.h"
#include "MageStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Mage
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterMageTriggers(context);
            RegisterMageStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "arcane"; // DPS
                case 1: return "fire";   // DPS
                case 2: return "frost";  // DPS
                default: return "frost";
            }
        }
    }
}
