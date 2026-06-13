/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  DK context bundle (S7). Layers the Death Knight triggers + spec strategies
 *  onto a bot's base AiObjectContext, and exposes the combat-strategy name for
 *  a DK's current spec (so AiFactory installs blood/frost/unholy correctly).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DK_AIOBJECTCONTEXT_H
#define PSYCHOBOT_DK_AIOBJECTCONTEXT_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class AiObjectContext;

    namespace DeathKnight
    {
        // Register DK triggers + strategies into the base context (S7).
        void RegisterContext(AiObjectContext* context);

        // The combat strategy name for a DK spec OrderIndex (0=Blood,1=Frost,
        // 2=Unholy). Falls back to "frost" for an unknown index.
        std::string CombatStrategyForSpec(uint8 specIndex);
    }
}

#endif // PSYCHOBOT_DK_AIOBJECTCONTEXT_H
