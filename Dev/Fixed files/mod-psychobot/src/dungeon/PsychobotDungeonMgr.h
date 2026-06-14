/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Dungeon / Raid scripted strategies (S25) - boss-mechanic awareness.
 *  FRAMEWORK + a few key encounters. The statically-safe, generic behaviours
 *  ship now (they work in every dungeon/raid without per-encounter data):
 *    InDungeonOrRaid   - is the bot in an instance?
 *    FightingBoss      - is the bot's current target a dungeon/world boss?
 *    BossCastingInterruptible - the boss is casting an interruptible spell now
 *                        -> the bot should kick it (wired as a trigger).
 *
 *  Per-encounter scripts (move out of THIS ground effect, spread/stack, soak,
 *  switch to THIS add) need encounter-specific positions/spell ids that can't be
 *  resolved statically. They are an EncounterScript registry (RegisterEncounter
 *  by creature entry) with a couple of seeded example entries; the concrete
 *  reactions are documented hooks for the first on-server pass.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DUNGEON_MGR_H
#define PSYCHOBOT_DUNGEON_MGR_H

#include "Define.h"
#include <functional>
#include <string>

class Player;
class Unit;

namespace psychobot
{
    namespace DungeonMgr
    {
        // Is the bot inside a dungeon or raid instance?
        bool InDungeonOrRaid(Player* bot);

        // Is the bot's current target a dungeon boss / world boss?
        bool FightingBoss(Player* bot);

        // Is the bot's current target (a boss) casting an interruptible spell
        // right now? -> the bot should interrupt it.
        bool BossCastingInterruptible(Player* bot);

        // --- per-encounter script framework (seeded; on-server expansion) ----
        // An encounter reaction: given the bot + the boss, return true if it took
        // a mechanic action this tick (e.g. moved out of fire). Default scripts
        // are no-op hooks until wired against live encounter data.
        using EncounterScript = std::function<bool(Player* bot, Unit* boss)>;

        // Register a per-boss script by creature entry.
        void RegisterEncounter(uint32 creatureEntry, EncounterScript script);

        // Run the registered script for the boss the bot is fighting (if any).
        // Returns true if the script handled a mechanic this tick.
        bool RunEncounterScript(Player* bot);

        // Seed the example encounter registry (called once at startup).
        void InitEncounters();
    }
}

#endif // PSYCHOBOT_DUNGEON_MGR_H
