/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  ActionBasket - one entry in the engine's relevance-priority queue: an action
 *  node + its relevance + the event that pushed it + a created-timestamp (for
 *  expiry). Highest relevance is executed first each tick.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_ACTIONBASKET_H
#define PSYCHOBOT_ENGINE_ACTIONBASKET_H

#include "Event.h"
#include <ctime>

namespace psychobot
{
    class ActionNode;   // forward-declared; defined in S2 (ActionNode.h)

    class ActionBasket
    {
    public:
        ActionBasket(ActionNode* action, float relevance, bool skipPrerequisites, Event const& event)
            : _action(action), _relevance(relevance),
              _skipPrerequisites(skipPrerequisites), _event(event), _created(time(nullptr)) { }

        float       GetRelevance() const { return _relevance; }
        ActionNode* GetAction()    const { return _action; }
        Event const& GetEvent()    const { return _event; }
        bool        IsSkipPrerequisites() const { return _skipPrerequisites; }

        void  AmendRelevance(float k)     { _relevance *= k; }
        void  SetRelevance(float r)       { _relevance = r; }
        void  SetEvent(Event const& e)    { _event = e; }
        bool  IsExpired(time_t secs) const { return time(nullptr) - _created >= secs; }

    private:
        ActionNode* _action;
        float       _relevance;
        bool        _skipPrerequisites;
        Event       _event;
        time_t      _created;
    };
}

#endif // PSYCHOBOT_ENGINE_ACTIONBASKET_H
