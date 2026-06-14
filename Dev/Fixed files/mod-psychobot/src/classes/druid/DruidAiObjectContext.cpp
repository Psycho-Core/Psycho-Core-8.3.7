/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "DruidAiObjectContext.h"
#include "DruidTriggers.h"
#include "DruidStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Druid
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterDruidTriggers(context);
            RegisterDruidStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "balance";     // DPS (ranged)
                case 1: return "feral";       // DPS (melee)
                case 2: return "guardian";    // Tank
                case 3: return "restoration"; // Healer
                default: return "balance";
            }
        }
    }
}
