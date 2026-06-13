/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotDbStore.h"
#include "DatabaseEnv.h"
#include "Log.h"

namespace psychobot
{
    PsychobotDbStore* PsychobotDbStore::instance()
    {
        static PsychobotDbStore instance;
        return &instance;
    }

    std::set<std::string> const& PsychobotDbStore::GetStrategies(ObjectGuid bot) const
    {
        auto it = _saved.find(bot);
        return it != _saved.end() ? it->second : _empty;
    }

    bool PsychobotDbStore::HasStrategy(ObjectGuid bot, std::string const& name) const
    {
        auto it = _saved.find(bot);
        return it != _saved.end() && it->second.count(name) != 0;
    }

    void PsychobotDbStore::SetStrategy(ObjectGuid bot, std::string const& name, bool on)
    {
        if (on)
            _saved[bot].insert(name);
        else
        {
            auto it = _saved.find(bot);
            if (it != _saved.end())
                it->second.erase(name);
        }
        Save(bot);   // persist (hook)
    }

    bool PsychobotDbStore::ToggleStrategy(ObjectGuid bot, std::string const& name)
    {
        bool nowOn = !HasStrategy(bot, name);
        SetStrategy(bot, name, nowOn);
        return nowOn;
    }

    void PsychobotDbStore::Clear(ObjectGuid bot)
    {
        _saved.erase(bot);
    }

    // --- persistence (characters DB: table psychobot_strategies) ----------
    // Requires: modules/mod-psychobot/sql/characters/psychobot_strategies.sql
    // applied to the characters database. Uses direct (auto-escaped) queries so
    // the module needs no registered prepared statements.
    void PsychobotDbStore::Save(ObjectGuid bot)
    {
        uint64 const guid = bot.GetCounter();

        // Replace the bot's whole set: clear, then re-insert the enabled names.
        CharacterDatabase.PExecute("DELETE FROM psychobot_strategies WHERE guid = " UI64FMTD, guid);

        auto it = _saved.find(bot);
        if (it == _saved.end())
            return;

        for (std::string const& name : it->second)
        {
            std::string escaped = name;
            CharacterDatabase.EscapeString(escaped);
            CharacterDatabase.PExecute(
                "INSERT INTO psychobot_strategies (guid, name) VALUES (" UI64FMTD ", '%s')",
                guid, escaped.c_str());
        }
    }

    void PsychobotDbStore::Load(ObjectGuid bot)
    {
        uint64 const guid = bot.GetCounter();

        QueryResult result = CharacterDatabase.PQuery(
            "SELECT name FROM psychobot_strategies WHERE guid = " UI64FMTD, guid);

        std::set<std::string>& set = _saved[bot];
        set.clear();
        if (!result)
            return;

        do
        {
            Field* fields = result->Fetch();
            set.insert(fields[0].GetString());
        }
        while (result->NextRow());
    }
}
