/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotPvpMgr.h"
#include "Player.h"
#include "Unit.h"
#include "Battleground.h"
#include "SharedDefines.h"   // BattlegroundTypeId

namespace psychobot
{
    namespace PvpMgr
    {
        bool InActiveBattleground(Player* bot)
        {
            if (!bot || !bot->InBattleground())
                return false;
            Battleground* bg = bot->GetBattleground();
            return bg && bg->GetStatus() == STATUS_IN_PROGRESS;
        }

        bool InArena(Player* bot)
        {
            return bot && bot->InArena();
        }

        BgFlavor GetBgFlavor(Player* bot)
        {
            if (!bot || !bot->InBattleground())
                return BgFlavor::None;

            if (bot->InArena())
                return BgFlavor::Arena;

            switch (bot->GetBattlegroundTypeId())
            {
                case BATTLEGROUND_WS:   // Warsong Gulch
                case BATTLEGROUND_TP:   // Twin Peaks
                case BATTLEGROUND_EY:   // Eye of the Storm (flag)
                case BATTLEGROUND_TK:   // Temple of Kotmogu (orbs)
                    return BgFlavor::FlagCarry;

                case BATTLEGROUND_AB:   // Arathi Basin
                case BATTLEGROUND_BFG:  // Battle for Gilneas
                case BATTLEGROUND_DG:   // Deepwind Gorge
                    return BgFlavor::BaseCapture;

                case BATTLEGROUND_AV:   // Alterac Valley
                case BATTLEGROUND_IC:   // Isle of Conquest
                    return BgFlavor::Resource;

                case BATTLEGROUND_SM:   // Silvershard Mines (carts)
                    return BgFlavor::Payload;

                default:
                    return BgFlavor::None;
            }
        }

        Unit* GetPvpTarget(Player* bot, float range)
        {
            if (!bot || !bot->IsInWorld())
                return nullptr;

            Player* enemy = bot->SelectNearestPlayer(range);
            if (!enemy || !enemy->IsAlive())
                return nullptr;
            if (!bot->IsValidAttackTarget(enemy))
                return nullptr;   // same faction / friendly
            return enemy;
        }

        // --- per-BG objective tactics (framework hooks) ---------------------
        bool DoFlagTactics(Player* /*bot*/)     { return false; }
        bool DoBaseTactics(Player* /*bot*/)     { return false; }
        bool DoResourceTactics(Player* /*bot*/) { return false; }
        bool DoArenaTactics(Player* /*bot*/)    { return false; }  // PvP target handles it
    }
}
