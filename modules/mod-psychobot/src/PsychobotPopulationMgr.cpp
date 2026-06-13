/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotPopulationMgr.h"
#include "PsychobotMgr.h"
#include "PsychobotLoginMgr.h"
#include "PsychobotFactory.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include "GameTime.h"
#include "Config.h"
#include "Log.h"
#include "DatabaseEnv.h"

namespace psychobot
{
    PsychobotPopulationMgr* PsychobotPopulationMgr::instance()
    {
        static PsychobotPopulationMgr instance;
        return &instance;
    }

    void PsychobotPopulationMgr::LoadConfig()
    {
        _cfg.enable                = sConfigMgr->GetBoolDefault ("Psychobot.RandomBots.Enable", false);
        _cfg.minRandomBots         = sConfigMgr->GetIntDefault  ("Psychobot.RandomBots.Min", 0);
        _cfg.maxRandomBots         = sConfigMgr->GetIntDefault  ("Psychobot.RandomBots.Max", 0);
        _cfg.randomBotMinLevel     = sConfigMgr->GetIntDefault  ("Psychobot.RandomBots.MinLevel", 1);
        _cfg.randomBotMaxLevel     = sConfigMgr->GetIntDefault  ("Psychobot.RandomBots.MaxLevel", 120);
        _cfg.syncLevelWithPlayers  = sConfigMgr->GetBoolDefault ("Psychobot.SyncLevelWithPlayers", false);

        _cfg.botActiveAlone        = sConfigMgr->GetIntDefault  ("Psychobot.ActiveAlone", 100);
        _cfg.activeAloneDuration   = sConfigMgr->GetIntDefault  ("Psychobot.ActiveAloneDuration", 120);
        _cfg.forceActiveRadius     = sConfigMgr->GetFloatDefault("Psychobot.ActiveAloneRadius", 200.0f);
        _cfg.smartScale            = sConfigMgr->GetBoolDefault ("Psychobot.SmartScale", true);
        _cfg.smartScaleDiffFloor   = sConfigMgr->GetIntDefault  ("Psychobot.SmartScaleDiffFloor", 50);
        _cfg.smartScaleDiffCeiling = sConfigMgr->GetIntDefault  ("Psychobot.SmartScaleDiffCeiling", 150);

        _cfg.disabledWithoutRealPlayer = sConfigMgr->GetBoolDefault("Psychobot.DisabledWithoutRealPlayer", true);
        _cfg.loginDelaySeconds     = sConfigMgr->GetIntDefault  ("Psychobot.LoginDelay", 0);
        _cfg.logoutDelaySeconds    = sConfigMgr->GetIntDefault  ("Psychobot.LogoutDelay", 0);

        _cfg.gearQualityLimit      = sConfigMgr->GetIntDefault  ("Psychobot.GearQualityLimit", 3);
        _cfg.preferClassArmorType  = sConfigMgr->GetBoolDefault ("Psychobot.PreferClassArmorType", true);
        _cfg.equipmentPersistence  = sConfigMgr->GetBoolDefault ("Psychobot.EquipmentPersistence", true);
        _cfg.equipmentPersistenceLevel = sConfigMgr->GetIntDefault("Psychobot.EquipmentPersistenceLevel", 0);

        // Clamp percentages.
        if (_cfg.botActiveAlone > 100) _cfg.botActiveAlone = 100;
    }

    bool PsychobotPopulationMgr::HasRealPlayerOnline() const
    {
        // A "real" player is any online session whose player is NOT a managed bot.
        for (auto const& pair : sWorld->GetAllSessions())
        {
            WorldSession* session = pair.second;
            if (!session)
                continue;
            Player* player = session->GetPlayer();
            if (!player || !player->IsInWorld())
                continue;
            if (!sPsychobotMgr->IsBot(player->GetGUID()))
                return true;
        }
        return false;
    }

    bool PsychobotPopulationMgr::IsRealPlayerNear(Player* bot) const
    {
        if (!bot)
            return false;
        for (auto const& pair : sWorld->GetAllSessions())
        {
            WorldSession* session = pair.second;
            if (!session)
                continue;
            Player* player = session->GetPlayer();
            if (!player || !player->IsInWorld())
                continue;
            if (sPsychobotMgr->IsBot(player->GetGUID()))
                continue;
            // Same map + within force radius, OR same zone.
            if (player->GetMap() == bot->GetMap() &&
                bot->GetDistance(player) <= _cfg.forceActiveRadius)
                return true;
            if (player->GetMap() == bot->GetMap() &&
                player->GetZoneId() == bot->GetZoneId())
                return true;
        }
        return false;
    }

    uint32 PsychobotPopulationMgr::EffectiveActivePercent() const
    {
        uint32 pct = _cfg.botActiveAlone;
        if (!_cfg.smartScale)
            return pct;

        // SmartScale: linearly reduce active% as world tick ms rises from floor
        // to ceiling. At/above ceiling -> 0% extra (only force-active bots run).
        uint32 diff = _lastWorldDiff;
        if (diff <= _cfg.smartScaleDiffFloor)
            return pct;
        if (diff >= _cfg.smartScaleDiffCeiling || _cfg.smartScaleDiffCeiling <= _cfg.smartScaleDiffFloor)
            return 0;

        uint32 range = _cfg.smartScaleDiffCeiling - _cfg.smartScaleDiffFloor;
        uint32 over  = diff - _cfg.smartScaleDiffFloor;
        // remaining fraction of pct
        return pct - (pct * over / range);
    }

    void PsychobotPopulationMgr::RotateActiveRoster()
    {
        _activeRoster.clear();
        // NOTE: With Phase A (socketless population) this selects a rotating
        // subset of the random-bot pool. Until then the roster is empty and
        // ShouldBotBeActive falls back to "active" for master-owned bots.
        _lastRosterRotation = GameTime::GetGameTime();
    }

    bool PsychobotPopulationMgr::IsInActiveRoster(ObjectGuid guid) const
    {
        return _activeRoster.find(guid) != _activeRoster.end();
    }

    bool PsychobotPopulationMgr::ShouldBotBeActive(Player* bot) const
    {
        if (!bot)
            return false;

        // If random-bot scaling is off, every managed bot is always active
        // (Stage 1/2 behaviour for master-owned alts).
        if (!_cfg.enable)
            return true;

        // DisabledWithoutRealPlayer: nobody real online -> bots idle.
        if (_cfg.disabledWithoutRealPlayer && !HasRealPlayerOnline())
            return false;

        // Force-active near/with a real player regardless of roster/scale.
        if (IsRealPlayerNear(bot))
            return true;

        // Otherwise only the active roster ticks (subject to SmartScale).
        return IsInActiveRoster(bot->GetGUID());
    }

    void PsychobotPopulationMgr::Update(uint32 diff)
    {
        _lastWorldDiff = diff;

        if (!_cfg.enable)
            return;

        if (_lastRealPlayerSeen == 0 || HasRealPlayerOnline())
            _lastRealPlayerSeen = GameTime::GetGameTime();

        // Rotate the active roster on its interval.
        _rosterAccum += diff;
        if (_rosterAccum >= _cfg.activeAloneDuration * 1000)
        {
            _rosterAccum = 0;
            RotateActiveRoster();
        }

        // Top up the random-bot pool toward minRandomBots (1 per tick to avoid lag)
        if (sPsychobotMgr->GetBotCount() < _cfg.minRandomBots)
        {
            bool foundOffline = false;
            static std::string cachedAccList = "";

            if (cachedAccList.empty())
            {
                QueryResult loginRes = LoginDatabase.Query("SELECT id FROM account WHERE username LIKE 'PSYCHOBOT_%'");
                if (loginRes)
                {
                    do
                    {
                        if (!cachedAccList.empty()) cachedAccList += ",";
                        cachedAccList += std::to_string(loginRes->Fetch()[0].GetUInt32());
                    } while (loginRes->NextRow());
                }
            }

            if (!cachedAccList.empty())
            {
                QueryResult charRes = CharacterDatabase.PQuery("SELECT guid, account FROM characters WHERE account IN (%s) AND online = 0 LIMIT 1", cachedAccList.c_str());
                if (charRes)
                {
                    ObjectGuid::LowType guidLow = charRes->Fetch()[0].GetUInt64();
                    uint32 accountId = charRes->Fetch()[1].GetUInt32();
                    ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(guidLow);

                    std::string reason;
                    LoginMgr::LoginBot(guid, accountId, reason);
                    foundOffline = true;
                }
            }

            if (!foundOffline)
            {
                // No offline bots available, so generate one
                if (sPsychobotFactory->GenerateOneBot())
                {
                    // Invalidate the account cache so we pick up the newly generated account next tick
                    cachedAccList = "";
                }
            }
        }
        (void)EffectiveActivePercent();
    }
}
