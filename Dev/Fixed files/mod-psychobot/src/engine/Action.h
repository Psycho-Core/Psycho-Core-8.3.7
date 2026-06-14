/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Action - one unit of bot behaviour. Subclasses override Execute(); the
 *  engine only runs an action when isPossible() && isUseful(). An action can
 *  also publish prerequisite / alternative / continuer NextAction chains so
 *  the engine can walk a small graph (e.g. "reach target" before "melee").
 *
 *  ActionNode (in ActionNode.h) wraps an Action with its own P/A/C overrides
 *  declared by a strategy via the ACTION_NODE_P/A/C macros.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_ACTION_H
#define PSYCHOBOT_ENGINE_ACTION_H

#include "AiObject.h"
#include "Event.h"
#include "NextAction.h"
#include "Define.h"
#include <string>

class Unit;

namespace psychobot
{
    template <class T> class Value;

    class Action : public AiNamedObject
    {
    public:
        Action(PsychobotAI* ai, std::string name = "action")
            : AiNamedObject(ai, std::move(name)) { }
        virtual ~Action() = default;

        // Do the thing. Return true if the action fired (consumes the tick).
        virtual bool Execute(Event const& event) = 0;

        // Cheap can-it-even-run gate (resources / range / known).
        virtual bool IsPossible() { return true; }
        // Should it be considered right now (context-sensitive)?
        virtual bool IsUseful() { return true; }
        // Still useful while the bot is stunned/feared? (default: no)
        virtual bool IsUsefulWhenStunned() { return false; }

        // P/A/C chains (default none; ActionNode overrides via macros).
        virtual NextAction** GetPrerequisites() { return nullptr; }
        virtual NextAction** GetAlternatives()  { return nullptr; }
        virtual NextAction** GetContinuers()    { return nullptr; }

        virtual ActionThreatType GetThreatType() { return ActionThreatType::ACTION_THREAT_NONE; }

        virtual void  Reset() { }
        virtual Unit* GetTarget();

        float GetRelevance() const { return _relevance; }
        void  SetRelevance(float r) { _relevance = r; }

    protected:
        float _relevance = 0.0f;
    };
}

#endif // PSYCHOBOT_ENGINE_ACTION_H
