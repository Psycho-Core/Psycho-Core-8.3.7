/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  AiObject - base for every engine object (actions/values/triggers/strategies)
 *  + the shared relevance-priority constants and action-threat enum.
 *  Clean-room re-implementation of the ike3/cmangos engine for TrinityCore 8.3.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_AIOBJECT_H
#define PSYCHOBOT_ENGINE_AIOBJECT_H

#include "Define.h"
#include <string>

class Player;

namespace psychobot
{
    class PsychobotAI;

    // ----------------------------------------------------------------------
    // Action relevance tiers (higher = picked first by the engine queue).
    // Mirrors the reference ActionPriority scale so ported strategies that use
    // ACTION_HIGH+2 / ACTION_EMERGENCY etc. translate 1:1.
    // ----------------------------------------------------------------------
    enum ActionPriority
    {
        ACTION_IDLE          = 1,
        ACTION_NORMAL        = 10,
        ACTION_HIGH          = 20,
        ACTION_MOVE          = 30,
        ACTION_INTERRUPT     = 40,
        ACTION_DISPEL        = 50,
        ACTION_LIGHT_HEAL    = 60,
        ACTION_MEDIUM_HEAL   = 70,
        ACTION_CRITICAL_HEAL = 80,
        ACTION_EMERGENCY     = 90,
        ACTION_PASSTHROUGH   = 100
    };

    // Threat contribution of an action (used by tank/aggro logic later).
    enum class ActionThreatType : uint8
    {
        ACTION_THREAT_NONE   = 0,
        ACTION_THREAT_SINGLE = 1,
        ACTION_THREAT_AOE    = 2
    };

    // ----------------------------------------------------------------------
    // AiObject - holds the owning AI + convenience accessors.
    // ----------------------------------------------------------------------
    class AiObject
    {
    public:
        explicit AiObject(PsychobotAI* ai) : _ai(ai) { }
        virtual ~AiObject() { }

        PsychobotAI* GetAI() const { return _ai; }

    protected:
        Player* GetBot() const;       // resolved from _ai (defined in .cpp)
        Player* GetMaster() const;    // resolved from _ai (defined in .cpp)

        PsychobotAI* _ai;
    };

    // ----------------------------------------------------------------------
    // AiNamedObject - an AiObject that carries a name (used by the registry).
    // ----------------------------------------------------------------------
    class AiNamedObject : public AiObject
    {
    public:
        AiNamedObject(PsychobotAI* ai, std::string name)
            : AiObject(ai), _name(std::move(name)) { }

        virtual std::string GetName() const { return _name; }

    protected:
        std::string _name;
    };
}

#endif // PSYCHOBOT_ENGINE_AIOBJECT_H
