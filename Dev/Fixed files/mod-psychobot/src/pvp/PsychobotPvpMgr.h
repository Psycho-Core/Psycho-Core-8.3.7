/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PvP / Battleground coordination (S24). Detects the BG/arena context and
 *  classifies its objective flavor, and supplies a PvP target (nearest enemy
 *  player) so bots actually FIGHT in instanced PvP and open-world PvP.
 *
 *    BgFlavor       - how the BG is won (the per-BG tactics branch on this).
 *    GetBgFlavor    - classify the bot's current BG from its TypeId.
 *    InActiveBattleground - true while a joined BG is actually running.
 *    GetPvpTarget   - nearest valid enemy player within range (the assist /
 *                     combat target seed when in PvP).
 *
 *  Per-BG objective tactics (carry the flag, capture the base, take the boss,
 *  man the vehicle) are FRAMEWORK HOOKS: they need live BG objective objects +
 *  positions that vary per battleground and can't be resolved statically, so
 *  they're gated/documented for the first on-server pass. The always-safe PvP
 *  behaviour (find + kill the nearest enemy, stay with the group) ships now.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PVP_MGR_H
#define PSYCHOBOT_PVP_MGR_H

#include "Define.h"

class Player;
class Unit;

namespace psychobot
{
    enum class BgFlavor : uint8
    {
        None        = 0,
        FlagCarry   = 1,   // WSG / Twin Peaks / Eye of the Storm (flag/orb)
        BaseCapture = 2,   // Arathi Basin / Battle for Gilneas / Deepwind Gorge
        Resource    = 3,   // Alterac Valley / Isle of Conquest (bases + boss/vehicles)
        Payload     = 4,   // Silvershard Mines (cart) / objective-carts
        Arena       = 5    // 2v2/3v3/skirmish - pure deathmatch
    };

    namespace PvpMgr
    {
        // True while the bot is in a battleground that's actually in progress.
        bool InActiveBattleground(Player* bot);

        // True if the bot is in an arena.
        bool InArena(Player* bot);

        // Classify the bot's current BG by its type id.
        BgFlavor GetBgFlavor(Player* bot);

        // The nearest valid enemy PLAYER within `range`, or nullptr. Used as the
        // PvP combat target (and to seed assist when no group target exists).
        Unit* GetPvpTarget(Player* bot, float range = 60.0f);

        // --- per-BG objective tactics (framework hooks; on-server pass) ------
        // Returns true if the bot took an objective action this tick. Until
        // implemented against live BG objects, these are no-op (bots fight via
        // GetPvpTarget instead). One hook per flavor keeps the AI seam clean.
        bool DoFlagTactics(Player* bot);     // carry/return/defend the flag
        bool DoBaseTactics(Player* bot);     // capture/defend a base node
        bool DoResourceTactics(Player* bot); // AV/IC bases, boss, vehicles
        bool DoArenaTactics(Player* bot);    // focus/peel (uses GetPvpTarget now)
    }
}

#endif // PSYCHOBOT_PVP_MGR_H
