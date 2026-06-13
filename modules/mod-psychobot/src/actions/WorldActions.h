/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Non-combat WORLD actions (S22). Out-of-combat housekeeping driven by the
 *  non-combat engine via PsychobotWorldMgr:
 *    "rest"   - eat/drink to full when hurt and safe
 *    "repair" - self-repair worn gear when affordable
 *  (Vendor-sell / loot / quest / gather are WorldMgr framework hooks for the
 *  first on-server pass and are not yet wired as engine actions.)
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ACTIONS_WORLDACTIONS_H
#define PSYCHOBOT_ACTIONS_WORLDACTIONS_H

#include "../engine/Action.h"
#include "../engine/NamedObjectContext.h"

namespace psychobot
{
    class RestAction : public Action
    {
    public:
        RestAction(PsychobotAI* ai) : Action(ai, "rest") { }
        bool Execute(Event const& event) override;
        bool IsUseful() override;
    };

    class RepairAction : public Action
    {
    public:
        RepairAction(PsychobotAI* ai) : Action(ai, "repair") { }
        bool Execute(Event const& event) override;
        bool IsUseful() override;
    };

    class AiObjectContext;
    void RegisterWorldActions(AiObjectContext* context);
}

#endif // PSYCHOBOT_ACTIONS_WORLDACTIONS_H
