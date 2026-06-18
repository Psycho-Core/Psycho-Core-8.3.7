/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotMgr.h"
#include "PsychobotPopulationMgr.h"
#include "PsychobotGroupMgr.h"
#include "PsychobotLoginMgr.h"
#include "ai/PsychobotAI.h"
#include "ai/PsychobotTalentMgr.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "CharacterCache.h"
#include "Config.h"
#include "Log.h"
#include <cstdlib>

namespace psychobot
{
    PsychobotMgr* PsychobotMgr::instance()
    {
        static PsychobotMgr instance;
        return &instance;
    }

    bool PsychobotMgr::IsBot(ObjectGuid guid) const
    {
        return _bots.find(guid) != _bots.end();
    }

    bool PsychobotMgr::AttachAI(Player* bot, ObjectGuid masterGuid)
    {
        if (!bot)
            return false;
        if (IsBot(bot->GetGUID()))
            return false;
        _bots[bot->GetGUID()] = std::make_unique<PsychobotAI>(bot, masterGuid);
        return true;
    }

    void PsychobotMgr::DetachAI(ObjectGuid botGuid)
    {
        _bots.erase(botGuid);
    }

    std::string PsychobotMgr::AddBot(Player* master, std::string const& charName)
    {
        if (!sConfigMgr->GetBoolDefault("Psychobot.Enable", false))
            return "Psychobot is disabled (set Psychobot.Enable = 1).";

        if (!master)
            return "No master.";

        if (charName.empty())
            return "Usage: .psychobot add <charactername>";

        // S28: if the target character is offline, log it in socketlessly as a
        // bot first. The character loads on a later world tick, so AttachAI then
        // happens when its OnLogin fires (handled by the module login hook), or
        // the master can re-issue .psychobot add once it's in world. We report
        // the in-progress login here.
        Player* bot = ObjectAccessor::FindConnectedPlayerByName(charName);
        if (!bot)
        {
            // Resolve the offline character so we can remember its master and
            // attach AI automatically when it finishes loading (OnPlayerLogin).
            CharacterCacheEntry const* entry = sCharacterCache->GetCharacterCacheByName(charName);
            if (!entry)
                return "No character named '" + charName + "' exists.";
            if (entry->Guid == master->GetGUID())
                return "You cannot add yourself as a bot.";
            if (IsBot(entry->Guid))
                return "'" + charName + "' is already a Psychobot.";

            std::string reason;
            if (LoginMgr::LoginBot(entry->Guid, entry->AccountId, reason))
            {
                _pendingMasters[entry->Guid] = master->GetGUID();
                return "Logging in '" + charName + "' as a Psychobot... " + reason
                     + " (it will start following once loaded).";
            }
            return "Could not bring '" + charName + "' online: " + reason;
        }

        if (bot->GetGUID() == master->GetGUID())
            return "You cannot add yourself as a bot.";

        if (IsBot(bot->GetGUID()))
            return "'" + charName + "' is already a Psychobot.";

        if (AttachAI(bot, master->GetGUID()))
            return "Added Psychobot: " + bot->GetName() + ". It will follow you.";

        return "Failed to add Psychobot.";
    }

    std::string PsychobotMgr::SummonBot(Player* master, std::string const& charName)
    {
        if (!sConfigMgr->GetBoolDefault("Psychobot.Enable", false))
            return "Psychobot is disabled (set Psychobot.Enable = 1).";

        if (!master)
            return "No master.";

        if (charName.empty())
            return "Usage: .psychobot summon <charactername>";

        Player* bot = ObjectAccessor::FindConnectedPlayerByName(charName);
        if (!bot)
        {
            // Offline: bring the character in via the same socketless login path
            // as AddBot and remember its master. AI attaches on OnPlayerLogin; the
            // bot is teleported to the master once it is fully in world (next tick).
            CharacterCacheEntry const* entry = sCharacterCache->GetCharacterCacheByName(charName);
            if (!entry)
                return "No character named '" + charName + "' exists.";
            if (entry->Guid == master->GetGUID())
                return "You cannot summon yourself.";

            std::string reason;
            if (LoginMgr::LoginBot(entry->Guid, entry->AccountId, reason))
            {
                _pendingMasters[entry->Guid] = master->GetGUID();
                return "Summoning '" + charName + "'... " + reason
                     + " (it will appear beside you once loaded).";
            }
            return "Could not bring '" + charName + "' online: " + reason;
        }

        if (bot->GetGUID() == master->GetGUID())
            return "You cannot summon yourself.";

        // Already in world: ensure AI is attached (idempotent) and teleport it
        // straight to the master's location.
        if (!IsBot(bot->GetGUID()))
            AttachAI(bot, master->GetGUID());

        bot->TeleportTo(master->GetMapId(), master->GetPositionX(),
            master->GetPositionY(), master->GetPositionZ(), master->GetOrientation());
        return "Summoned Psychobot: " + bot->GetName() + ".";
    }

    std::string PsychobotMgr::RemoveBot(Player* master, std::string const& charName)
    {
        if (!master)
            return "No master.";

        if (charName.empty())
            return "Usage: .psychobot remove <charactername>";

        Player* bot = ObjectAccessor::FindConnectedPlayerByName(charName);
        if (bot && IsBot(bot->GetGUID()))
        {
            ObjectGuid guid = bot->GetGUID();
            std::string name = bot->GetName();
            DetachAI(guid);
            // S28: if this bot was brought online socketlessly, log it back out
            // of the world (real players we manage as bots are left connected).
            if (LoginMgr::HasBotSession(guid))
                LoginMgr::LogoutBot(guid);
            return "Removed Psychobot: " + name + ".";
        }
        return "'" + charName + "' is not an active Psychobot.";
    }

    std::string PsychobotMgr::ListBots(Player* master)
    {
        if (_bots.empty())
            return "No active Psychobots.";

        std::string out = "Active Psychobots (" + std::to_string(_bots.size()) + "):";
        for (auto const& pair : _bots)
        {
            if (PsychobotAI* ai = pair.second.get())
                if (Player* bot = ai->GetBot())
                    out += "\n  - " + bot->GetName();
        }
        return out;
    }

    std::string PsychobotMgr::SetSpec(Player* /*master*/, std::string const& args)
    {
        // Parse "<charname> <specIndex>".
        std::string name = args;
        int32 specIndex = -1;
        size_t sp = args.find_last_of(' ');
        if (sp != std::string::npos)
        {
            name = args.substr(0, sp);
            specIndex = std::atoi(args.substr(sp + 1).c_str());
        }
        if (name.empty())
            return "Usage: .psychobot spec <charactername> <specIndex 0-3>";

        Player* bot = ObjectAccessor::FindConnectedPlayerByName(name);
        if (!bot || !IsBot(bot->GetGUID()))
            return "'" + name + "' is not an active Psychobot.";

        // Reset to allow re-speccing, then apply the requested spec.
        bot->SetPrimarySpecialization(0);
        TalentMgr::SetupSpec(bot, specIndex, 0);
        return "Re-specced Psychobot '" + bot->GetName() + "' to spec index "
             + std::to_string(specIndex) + ".";
    }

    std::string PsychobotMgr::GroupBot(Player* master, std::string const& charName)
    {
        if (!master)
            return "No master.";
        if (charName.empty())
            return "Usage: .psychobot group <charactername>";

        Player* bot = ObjectAccessor::FindConnectedPlayerByName(charName);
        if (!bot || !IsBot(bot->GetGUID()))
            return "'" + charName + "' is not an active Psychobot.";

        if (BotGroupMgr::InviteToGroup(master, bot))
            return "Added '" + bot->GetName() + "' to your party (role assigned).";
        return "Could not add '" + bot->GetName() + "' to your party (full / already grouped?).";
    }

    // ----------------------------------------------------------------------
    // S27 chat-command grammar - apply an order to every bot owned by `master`.
    // ----------------------------------------------------------------------
    namespace
    {
        // Count of bots affected, via a per-bot callback.
        template <class Fn>
        uint32 ForEachOwnedBot(std::unordered_map<ObjectGuid, std::unique_ptr<PsychobotAI>>& bots,
                               Player* master, Fn&& fn)
        {
            if (!master)
                return 0;
            ObjectGuid mg = master->GetGUID();
            uint32 n = 0;
            for (auto& pair : bots)
            {
                PsychobotAI* ai = pair.second.get();
                if (ai && ai->GetMasterGuid() == mg)
                    if (fn(ai))
                        ++n;
            }
            return n;
        }
    }

    std::string PsychobotMgr::OrderFollow(Player* master)
    {
        uint32 n = ForEachOwnedBot(_bots, master, [](PsychobotAI* ai) { return ai->OrderFollow(); });
        return std::to_string(n) + " bot(s) now following.";
    }

    std::string PsychobotMgr::OrderStay(Player* master)
    {
        uint32 n = ForEachOwnedBot(_bots, master, [](PsychobotAI* ai) { return ai->OrderStay(); });
        return std::to_string(n) + " bot(s) holding position.";
    }

    std::string PsychobotMgr::OrderAttack(Player* master)
    {
        if (!master || !master->GetSelectedUnit())
            return "Select an enemy target first.";
        uint32 n = ForEachOwnedBot(_bots, master, [](PsychobotAI* ai) { return ai->OrderAttackMasterTarget(); });
        return std::to_string(n) + " bot(s) attacking your target.";
    }

    std::string PsychobotMgr::OrderCast(Player* master, std::string const& spellName)
    {
        if (spellName.empty())
            return "Usage: .psychobot cast <spell name>";
        std::string spell = spellName;
        uint32 n = ForEachOwnedBot(_bots, master, [&spell](PsychobotAI* ai) { return ai->OrderCast(spell); });
        return std::to_string(n) + " bot(s) cast '" + spellName + "'.";
    }

    std::string PsychobotMgr::ToggleStrategy(Player* master, std::string const& args)
    {
        if (args.empty())
            return "Usage: .psychobot strategy <name>   (toggles e.g. 'tank', 'ranged')";
        std::string name = args;
        bool last = false;
        uint32 n = ForEachOwnedBot(_bots, master, [&name, &last](PsychobotAI* ai)
        {
            last = ai->ToggleCombatStrategy(name);
            return true;
        });
        return std::to_string(n) + " bot(s): strategy '" + name + "' " + (last ? "ON" : "OFF") + ".";
    }

    void PsychobotMgr::OnPlayerLogout(Player* player)
    {
        if (!player)
            return;

        // If a bot logs out, drop its AI.
        DetachAI(player->GetGUID());

        // If a master logs out, drop the bots it owns.
        ObjectGuid masterGuid = player->GetGUID();
        for (auto it = _bots.begin(); it != _bots.end(); )
        {
            if (it->second && it->second->GetMasterGuid() == masterGuid)
                it = _bots.erase(it);
            else
                ++it;
        }
    }

    void PsychobotMgr::OnPlayerLogin(Player* player)
    {
        if (!player)
            return;

        // Is this the freshly-loaded socketless bot a master just requested?
        auto it = _pendingMasters.find(player->GetGUID());
        if (it == _pendingMasters.end())
            return;

        ObjectGuid masterGuid = it->second;
        _pendingMasters.erase(it);

        if (AttachAI(player, masterGuid))
            TC_LOG_INFO("module.psychobot", "[Psychobot] %s finished socketless login; AI attached.",
                player->GetName().c_str());
    }

    void PsychobotMgr::UpdateAI(uint32 diff)
    {
        // Stage 3: drive the population/scaling system every tick (it decides
        // which bots are active and, with Phase A, tops up the random-bot pool).
        sPsychobotPopulation->Update(diff);

        if (_bots.empty())
            return;

        for (auto it = _bots.begin(); it != _bots.end(); )
        {
            PsychobotAI* ai = it->second.get();
            // Drop AI whose bot left the world (logged out / deleted).
            if (!ai || !ai->GetBot() || !ai->GetBot()->IsInWorld())
            {
                it = _bots.erase(it);
                continue;
            }
            ai->UpdateAI(diff);
            ++it;
        }
    }
}
