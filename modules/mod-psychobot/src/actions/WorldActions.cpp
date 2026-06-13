/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "WorldActions.h"
#include "../engine/AiObjectContext.h"
#include "../engine/ServerFacade.h"
#include "../world/PsychobotWorldMgr.h"
#include "../PsychobotAIFwd.h"
#include "Player.h"

namespace psychobot
{
    // ----------------------------------------------------------------------
    // RestAction - eat/drink to full when hurt and safe.
    // ----------------------------------------------------------------------
    bool RestAction::IsUseful()
    {
        Player* bot = GetBot();
        if (!WorldMgr::CanRest(bot))
            return false;
        // Hurt on health, or (mana users) low on mana.
        if (bot->GetHealthPct() < 95.0f)
            return true;
        return ServerFacade::GetManaPct(bot) > 0.0f && ServerFacade::GetManaPct(bot) < 95.0f;
    }

    bool RestAction::Execute(Event const& /*event*/)
    {
        return WorldMgr::Rest(GetBot(), 95.0f);
    }

    // ----------------------------------------------------------------------
    // RepairAction - self-repair worn gear when affordable.
    // ----------------------------------------------------------------------
    bool RepairAction::IsUseful()
    {
        Player* bot = GetBot();
        return bot && bot->IsInWorld() && !bot->IsInCombat();
    }

    bool RepairAction::Execute(Event const& /*event*/)
    {
        return WorldMgr::Repair(GetBot(), 40.0f);
    }

    // ----------------------------------------------------------------------
    // Registration
    // ----------------------------------------------------------------------
    namespace
    {
        class WorldActionContext : public NamedObjectContext<Action>
        {
        public:
            WorldActionContext() : NamedObjectContext<Action>(/*shared*/ false)
            {
                _creators["rest"]   = [](PsychobotAI* ai) -> Action* { return new RestAction(ai); };
                _creators["repair"] = [](PsychobotAI* ai) -> Action* { return new RepairAction(ai); };
            }
        };
    }

    void RegisterWorldActions(AiObjectContext* context)
    {
        if (context)
            context->AddActionContext(new WorldActionContext());
    }
}
