/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "ShamanAiObjectContext.h"
#include "ShamanTriggers.h"
#include "ShamanStrategies.h"
#include "../../engine/AiObjectContext.h"

namespace psychobot
{
    namespace Shaman
    {
        void RegisterContext(AiObjectContext* context)
        {
            if (!context)
                return;
            RegisterShamanTriggers(context);
            RegisterShamanStrategies(context);
        }

        std::string CombatStrategyForSpec(uint8 specIndex)
        {
            switch (specIndex)
            {
                case 0: return "elemental";   // DPS (ranged)
                case 1: return "enhancement"; // DPS (melee)
                case 2: return "restoration"; // Healer
                default: return "elemental";
            }
        }
    }
}
