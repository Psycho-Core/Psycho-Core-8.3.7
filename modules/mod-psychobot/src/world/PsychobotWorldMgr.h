/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotWorldMgr - non-combat world behaviours (S22). Self-contained, safe
 *  out-of-combat housekeeping a bot does when it isn't fighting:
 *    Rest     - restore health (and mana for mana users) to full when hurt and
 *               out of combat (models eating/drinking; deterministic).
 *    Repair   - self-repair worn gear via Player::DurabilityRepairAll when below
 *               a durability threshold and the bot can afford it.
 *    Mount    - mount up to keep pace with a mounted/distant master (uses a
 *               known mount spell; see notes - needs the bot's mount collection).
 *
 *  Vendor-sell / loot / quest accept+turn-in / gather are FRAMEWORK HOOKS here:
 *  they need live world objects + packet flow and are gated/documented for the
 *  first on-server pass rather than shipped as risky half-implementations.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_WORLD_MGR_H
#define PSYCHOBOT_WORLD_MGR_H

#include "Define.h"

class Player;

namespace psychobot
{
    namespace WorldMgr
    {
        // True if the bot is in a safe state to do non-combat housekeeping
        // (in world, alive, not in combat, not in a battleground).
        bool CanRest(Player* bot);

        // Restore health (+ mana) to full when out of combat and below pct.
        // Returns true if it healed/refilled this call.
        bool Rest(Player* bot, float belowPct = 95.0f);

        // Self-repair all equipped items if average durability is below pct and
        // the bot can pay. Returns true if a repair happened.
        bool Repair(Player* bot, float belowPct = 40.0f);

        // Mount up to follow a distant/mounted master. Returns true if mounted.
        // (Framework hook: resolves a known mount; no-op if none available.)
        bool MountForTravel(Player* bot);

        // --- framework hooks (need live world objects; first on-server pass) --
        // Sell grey/junk items to a nearby vendor.  Loot a nearby corpse.
        // Accept/turn-in a quest at a nearby quest giver.  Gather a node.
        // All return false until implemented against a running server.
        bool SellJunk(Player* bot);
        bool LootNearby(Player* bot);
        bool HandleQuests(Player* bot);
        bool GatherNearby(Player* bot);
    }
}

#endif // PSYCHOBOT_WORLD_MGR_H
