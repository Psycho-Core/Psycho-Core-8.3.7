/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotDbStore (S27) - persists a bot's saved strategy overrides (extra
 *  combat strategies the master toggled on, e.g. an off-spec or utility pack)
 *  keyed by the bot's character GUID, so they survive a relog. Clean-room
 *  re-impl of the reference PlayerbotDbStore concept.
 *
 *  The in-memory store + the get/set/toggle API are REAL and verifiable now.
 *  The actual DB read/write (Save/Load) is a documented hook: it needs a
 *  characters-DB table + prepared statements that can only be exercised against
 *  a running server, so it's gated there. In memory it works for the session.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_DB_STORE_H
#define PSYCHOBOT_DB_STORE_H

#include "ObjectGuid.h"
#include <set>
#include <string>
#include <unordered_map>

namespace psychobot
{
    class PsychobotDbStore
    {
    public:
        static PsychobotDbStore* instance();

        // The extra (master-toggled) strategy names saved for this bot.
        std::set<std::string> const& GetStrategies(ObjectGuid bot) const;

        // Toggle a strategy on/off for a bot; returns the new state (true=on).
        bool ToggleStrategy(ObjectGuid bot, std::string const& name);
        void SetStrategy(ObjectGuid bot, std::string const& name, bool on);
        bool HasStrategy(ObjectGuid bot, std::string const& name) const;

        // Forget a bot's saved set (e.g. on removal).
        void Clear(ObjectGuid bot);

        // --- persistence hooks (on-server pass) -----------------------------
        // Save/Load the bot's saved strategies to/from the characters DB. These
        // are documented hooks (need a DB table + prepared statements); the
        // in-memory store above already keeps them for the running session.
        void Save(ObjectGuid bot);
        void Load(ObjectGuid bot);

    private:
        PsychobotDbStore() = default;

        std::unordered_map<ObjectGuid, std::set<std::string>> _saved;
        std::set<std::string> _empty;   // returned for unknown bots
    };
}

#define sPsychobotDbStore psychobot::PsychobotDbStore::instance()

#endif // PSYCHOBOT_DB_STORE_H
