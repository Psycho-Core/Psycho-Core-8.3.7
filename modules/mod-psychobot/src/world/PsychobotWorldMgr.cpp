/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotWorldMgr.h"
#include "Player.h"
#include "Unit.h"
#include "SharedDefines.h"   // POWER_MANA

namespace psychobot
{
    namespace WorldMgr
    {
        bool CanRest(Player* bot)
        {
            return bot && bot->IsInWorld() && bot->IsAlive()
                && !bot->IsInCombat() && !bot->InBattleground();
        }

        bool Rest(Player* bot, float belowPct)
        {
            if (!CanRest(bot))
                return false;

            bool did = false;

            // Eat: restore health if hurt.
            if (bot->GetHealthPct() < belowPct)
            {
                bot->SetFullHealth();
                did = true;
            }

            // Drink: restore mana if this bot uses mana and is low.
            if (bot->GetPowerType() == POWER_MANA)
            {
                if (bot->GetPowerPct(POWER_MANA) < belowPct)
                {
                    bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));
                    did = true;
                }
            }
            return did;
        }

        bool Repair(Player* bot, float belowPct)
        {
            if (!bot || !bot->IsInWorld() || bot->IsInCombat())
                return false;

            // DurabilityRepairAll(cost, discountMod, guildBank). We pay from the
            // bot's own coinage (cost=true) at full price (discountMod 0). The
            // core checks affordability and only repairs what can be paid for.
            uint32 spent = bot->DurabilityRepairAll(/*cost*/ true, /*discountMod*/ 0.0f, /*guildBank*/ false);
            (void)belowPct;   // threshold is advisory; the AI gates the call
            return spent > 0;
        }

        bool MountForTravel(Player* /*bot*/)
        {
            // Framework hook: mounting needs a mount spell from the bot's mount
            // collection (DB2/known spells) which varies per character. Wiring a
            // concrete mount is a first-on-server tuning task; returning false
            // here keeps the AI safe (it just runs to follow instead).
            return false;
        }

        // --- framework hooks (need live world objects; first on-server pass) --
        bool SellJunk(Player* /*bot*/)     { return false; }
        bool LootNearby(Player* /*bot*/)   { return false; }
        bool HandleQuests(Player* /*bot*/) { return false; }
        bool GatherNearby(Player* /*bot*/) { return false; }
    }
}
