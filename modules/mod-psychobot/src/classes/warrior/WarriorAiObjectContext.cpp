/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "WarriorAiObjectContext.h"
#include "WarriorTriggers.h"
#include "WarriorStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Warrior
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterWarriorTriggers(context);
            RegisterWarriorStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "arms";        // DPS
                case 1: return "fury";        // DPS
                case 2: return "protection";  // Tank
                default: return "arms";
            }
        }
    }
}
