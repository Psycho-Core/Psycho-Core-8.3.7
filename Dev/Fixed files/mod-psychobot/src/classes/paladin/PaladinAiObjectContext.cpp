/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PaladinAiObjectContext.h"
#include "PaladinTriggers.h"
#include "PaladinStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Paladin
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterPaladinTriggers(context);
            RegisterPaladinStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "holy";         // Healer
                case 1: return "protection";   // Tank
                case 2: return "retribution";  // DPS
                default: return "retribution";
            }
        }
    }
}
