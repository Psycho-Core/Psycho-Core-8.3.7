/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Event - context carried into an action/trigger evaluation. Holds a source
 *  string + optional param + optional owning Player. (Packet-driven events are
 *  added in a later step alongside the reaction/world-packet handlers.)
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_EVENT_H
#define PSYCHOBOT_ENGINE_EVENT_H

#include "ObjectGuid.h"
#include <string>

class Player;

namespace psychobot
{
    class Event
    {
    public:
        Event() = default;
        explicit Event(std::string source) : _source(std::move(source)) { }
        Event(std::string source, std::string param, Player* owner = nullptr)
            : _source(std::move(source)), _param(std::move(param)), _owner(owner) { }
        Event(std::string source, ObjectGuid object, Player* owner = nullptr)
            : _source(std::move(source)), _object(object), _owner(owner) { }

        std::string const& GetSource() const { return _source; }
        std::string const& GetParam()  const { return _param; }
        ObjectGuid  GetObject() const { return _object; }
        Player*     GetOwner()  const { return _owner; }

        // An empty source means "no event".
        bool operator!() const { return _source.empty(); }

    private:
        std::string _source;
        std::string _param;
        ObjectGuid  _object;
        Player*     _owner = nullptr;
    };
}

#endif // PSYCHOBOT_ENGINE_EVENT_H
