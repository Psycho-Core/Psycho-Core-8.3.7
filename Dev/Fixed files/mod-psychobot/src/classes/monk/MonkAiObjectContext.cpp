/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "MonkAiObjectContext.h"
#include "MonkTriggers.h"
#include "MonkStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Monk
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterMonkTriggers(context);
            RegisterMonkStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "brewmaster"; // Tank
                case 1: return "mistweaver"; // Healer
                case 2: return "windwalker"; // DPS
                default: return "windwalker";
            }
        }
    }
}
