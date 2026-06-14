/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "DemonHunterAiObjectContext.h"
#include "DemonHunterTriggers.h"
#include "DemonHunterStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace DemonHunter
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterDemonHunterTriggers(context);
            RegisterDemonHunterStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "havoc";     // DPS
                case 1: return "vengeance"; // Tank
                default: return "havoc";
            }
        }
    }
}
