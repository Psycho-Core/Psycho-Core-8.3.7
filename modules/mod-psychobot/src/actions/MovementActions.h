/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Movement + attack ACTIONS (S5).
 *    "reach melee"  - chase the current target into melee range
 *    "reach spell"  - move into the bot's ranged-cast range of the target
 *    "follow master"- the non-combat default (stay near the master)
 *    "stop"         - clear movement
 *    "attack"       - begin / maintain melee auto-attack on the current target
 *
 *  These are the prerequisites the CastSpellAction family pushes so a bot walks
 *  into range before it casts, and the backbone of the generic strategies (S6).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ACTIONS_MOVEMENTACTIONS_H
#define PSYCHOBOT_ACTIONS_MOVEMENTACTIONS_H

#include "../engine/Action.h"
#include "../engine/NamedObjectContext.h"

class Unit;

namespace psychobot
{
    // Chase the current target to within `distance` yards.
    class ReachTargetAction : public Action
    {
    public:
        ReachTargetAction(PsychobotAI* ai, std::string name, float distance)
            : Action(ai, std::move(name)), _distance(distance) { }
        bool  Execute(Event const& event) override;
        bool  IsUseful() override;
        Unit* GetTarget() override;
    protected:
        float _distance;
    };

    // Stay close to the master (non-combat default).
    class FollowMasterAction : public Action
    {
    public:
        FollowMasterAction(PsychobotAI* ai) : Action(ai, "follow master") { }
        bool Execute(Event const& event) override;
        bool IsUseful() override;
    };

    // Stop all movement.
    class StopMovingAction : public Action
    {
    public:
        StopMovingAction(PsychobotAI* ai) : Action(ai, "stop") { }
        bool Execute(Event const& event) override;
    };

    // Begin / maintain melee auto-attack on the current target.
    class AttackAction : public Action
    {
    public:
        AttackAction(PsychobotAI* ai) : Action(ai, "attack") { }
        bool  Execute(Event const& event) override;
        bool  IsUseful() override;
        Unit* GetTarget() override;
        ActionThreatType GetThreatType() override { return ActionThreatType::ACTION_THREAT_SINGLE; }
    };

    class AiObjectContext;
    void RegisterMovementActions(AiObjectContext* context);
}

#endif // PSYCHOBOT_ACTIONS_MOVEMENTACTIONS_H
