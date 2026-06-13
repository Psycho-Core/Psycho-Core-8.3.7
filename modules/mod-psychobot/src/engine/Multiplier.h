/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Multiplier - scales an action's relevance just before it's executed (e.g.
 *  "while feared, multiply all non-escape actions by 0"). A strategy installs
 *  multipliers; the engine multiplies every popped action's relevance by the
 *  product of all active multipliers. PassiveMultiplier is a no-op default.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_MULTIPLIER_H
#define PSYCHOBOT_ENGINE_MULTIPLIER_H

#include "AiObject.h"
#include <string>

namespace psychobot
{
    class Action;

    class Multiplier : public AiNamedObject
    {
    public:
        Multiplier(PsychobotAI* ai, std::string name = "multiplier")
            : AiNamedObject(ai, std::move(name)) { }
        virtual ~Multiplier() = default;

        // Return a scalar applied to the action's relevance (1.0 = no change,
        // 0.0 = suppress entirely).
        virtual float GetValue(Action* action) = 0;
    };

    // A multiplier that never changes anything (placeholder / base default).
    class PassiveMultiplier : public Multiplier
    {
    public:
        PassiveMultiplier(PsychobotAI* ai, std::string name = "passive")
            : Multiplier(ai, std::move(name)) { }

        float GetValue(Action* /*action*/) override { return 1.0f; }
    };
}

#endif // PSYCHOBOT_ENGINE_MULTIPLIER_H
