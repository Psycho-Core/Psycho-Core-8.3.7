/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "HunterAiObjectContext.h"
#include "HunterTriggers.h"
#include "HunterStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Hunter
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterHunterTriggers(context);
            RegisterHunterStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "beast mastery"; // DPS (ranged, pet)
                case 1: return "marksmanship";  // DPS (ranged)
                case 2: return "survival";      // DPS (melee, pet)
                default: return "beast mastery";
            }
        }
    }
}
