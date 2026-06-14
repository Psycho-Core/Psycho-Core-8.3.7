/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Trigger - a condition the engine polls each tick. When IsActive() is true,
 *  the trigger's bound NextAction[] handlers are pushed into the queue at their
 *  declared relevance. TriggerNode binds a (lazily-resolved) trigger name to
 *  the handler array a strategy supplies.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_TRIGGER_H
#define PSYCHOBOT_ENGINE_TRIGGER_H

#include "AiObject.h"
#include "Event.h"
#include "NextAction.h"
#include <string>

namespace psychobot
{
    class Trigger : public AiNamedObject
    {
    public:
        Trigger(PsychobotAI* ai, std::string name = "trigger")
            : AiNamedObject(ai, std::move(name)) { }
        virtual ~Trigger() = default;

        virtual bool IsActive() { return false; }

        // Build the Event a freshly-fired trigger hands to its actions.
        virtual Event GetEvent() { return Event(GetName()); }

        // Some triggers fire once then go quiet until reset.
        virtual void Reset() { }
    };

    // ----------------------------------------------------------------------
    // TriggerNode - binds a trigger name to its handler NextAction[] array.
    // The actual Trigger* is resolved lazily from the AiObjectContext.
    // ----------------------------------------------------------------------
    class TriggerNode
    {
    public:
        TriggerNode(std::string name, NextAction** handlers = nullptr)
            : _name(std::move(name)), _handlers(handlers), _trigger(nullptr) { }

        virtual ~TriggerNode() { NextAction::Destroy(_handlers); }

        std::string const& GetName() const { return _name; }
        NextAction**       GetHandlers() const { return _handlers; }

        Trigger* GetTrigger() const { return _trigger; }
        void     SetTrigger(Trigger* t) { _trigger = t; }

        float GetFirstRelevance() const
        {
            if (_handlers && _handlers[0])
                return _handlers[0]->GetRelevance();
            return 0.0f;
        }

    private:
        std::string _name;
        NextAction** _handlers;
        Trigger*    _trigger;   // not owned (lives in the context cache)
    };
}

#endif // PSYCHOBOT_ENGINE_TRIGGER_H
