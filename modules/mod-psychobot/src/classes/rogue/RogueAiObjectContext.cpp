/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "RogueAiObjectContext.h"
#include "RogueTriggers.h"
#include "RogueStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Rogue
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterRogueTriggers(context);
            RegisterRogueStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "assassination"; // DPS
                case 1: return "outlaw";        // DPS
                case 2: return "subtlety";      // DPS
                default: return "assassination";
            }
        }
    }
}
