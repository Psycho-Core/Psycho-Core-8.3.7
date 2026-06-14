/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotFactory.h"
#include "PsychobotLoginMgr.h"
#include "AccountMgr.h"
#include "CharacterCache.h"
#include "CharacterPackets.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Random.h"
#include "SharedDefines.h"
#include "Util.h"
#include "WorldSession.h"
#include "World.h"
#include "Config.h"
#include <memory>

namespace psychobot
{
    PsychobotFactory* PsychobotFactory::instance()
    {
        static PsychobotFactory instance;
        return &instance;
    }

    void PsychobotFactory::LoadNameCache()
    {
        if (!_maleNames.empty() || !_femaleNames.empty())
            return; // Already loaded

        QueryResult result = WorldDatabase.Query("SELECT name, gender FROM ai_playerbot_names");
        if (!result)
        {
            TC_LOG_ERROR("module.psychobot", "PsychobotFactory: Failed to load names from `ai_playerbot_names`. Table empty or missing!");
            return;
        }

        do {
            std::string name = result->Fetch()[0].GetString();
            uint8 gender = result->Fetch()[1].GetUInt8();
            if (gender == GENDER_MALE)
                _maleNames.push_back(name);
            else
                _femaleNames.push_back(name);
        } while (result->NextRow());

        TC_LOG_INFO("module.psychobot", "PsychobotFactory: Loaded %u male and %u female names into cache.", uint32(_maleNames.size()), uint32(_femaleNames.size()));
    }

    std::string PsychobotFactory::GetRandomNameFromCache(uint8 gender)
    {
        std::vector<std::string>* pool = (gender == GENDER_MALE) ? &_maleNames : &_femaleNames;
        if (pool->empty())
            return "";

        for (int attempt = 0; attempt < 50; ++attempt)
        {
            uint32 index = urand(0, pool->size() - 1);
            std::string name = (*pool)[index];

            // Check if name is already taken in the DB
            QueryResult res = CharacterDatabase.PQuery("SELECT 1 FROM characters WHERE name = '%s'", name.c_str());
            if (!res)
            {
                // Valid and untaken, remove it from the pool to avoid future collisions
                pool->erase(pool->begin() + index);
                return name;
            }
        }
        return "";
    }

    bool PsychobotFactory::GetValidRaceClassCombo(uint8& outRace, uint8& outClass) const
    {
        // 8.3.7 Classes
        std::vector<uint8> classes = {
            CLASS_WARRIOR, CLASS_PALADIN, CLASS_HUNTER, CLASS_ROGUE, CLASS_PRIEST,
            CLASS_DEATH_KNIGHT, CLASS_SHAMAN, CLASS_MAGE, CLASS_WARLOCK, CLASS_MONK,
            CLASS_DRUID, CLASS_DEMON_HUNTER
        };

        // 8.3.7 Base + Allied Races
        std::vector<uint8> races = {
            RACE_HUMAN, RACE_ORC, RACE_DWARF, RACE_NIGHT_ELF, RACE_UNDEAD, RACE_TAUREN,
            RACE_GNOME, RACE_TROLL, RACE_GOBLIN, RACE_BLOOD_ELF, RACE_DRAENEI, RACE_WORGEN,
            RACE_PANDAREN_ALLIANCE, RACE_PANDAREN_HORDE, RACE_NIGHTBORNE, RACE_HIGHMOUNTAIN_TAUREN,
            RACE_VOID_ELF, RACE_LIGHTFORGED_DRAENEI, RACE_ZANDALARI_TROLL, RACE_KUL_TIRAN,
            RACE_DARK_IRON_DWARF, RACE_MAGHAR_ORC
        };

        // Shuffle arrays conceptually by picking random
        for (int attempt = 0; attempt < 100; ++attempt)
        {
            uint8 c = classes[urand(0, classes.size() - 1)];
            uint8 r = races[urand(0, races.size() - 1)];

            // Check if TrinityCore says this race/class combo is legal
            if (sObjectMgr->GetClassExpansionRequirement(r, c) != nullptr)
            {
                outClass = c;
                outRace = r;
                return true;
            }
        }
        return false;
    }

    uint32 PsychobotFactory::GetOrCreateBotAccount(uint32 botIndex)
    {
        std::string accountName = "PSYCHOBOT_" + std::to_string(botIndex);

        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_ACCOUNT_ID_BY_USERNAME);
        stmt->setString(0, accountName);
        if (PreparedQueryResult result = LoginDatabase.Query(stmt))
        {
            return result->Fetch()[0].GetUInt32();
        }

        // Doesn't exist, create it natively
        std::string password = "";
        for (int i = 0; i < 16; i++) password += (char)urand('a', 'z');

        if (sAccountMgr->CreateAccount(accountName, password) == AccountOpResult::AOR_OK)
        {
            // Fetch ID
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_ACCOUNT_ID_BY_USERNAME);
            stmt->setString(0, accountName);
            if (PreparedQueryResult res = LoginDatabase.Query(stmt))
            {
                uint32 newId = res->Fetch()[0].GetUInt32();
                // Assign to BOT RBAC or just user (SEC_PLAYER = 0)
                LoginDatabase.PExecute("INSERT INTO rbac_account_permissions (accountId, permissionId, granted, realmId) VALUES (%u, 1, 1, -1)", newId);
                return newId;
            }
        }
        return 0;
    }

    bool PsychobotFactory::GenerateOneBot()
    {
        // 1. Figure out which account to use.
        // We will just find the first PSYCHOBOT_ account that has fewer than 10 characters.
        // Start from _nextAccountIndex (remembered from the last call) instead of
        // always rescanning from account #1 - this avoids re-checking hundreds of
        // already-full accounts every time a single bot is generated.
        uint32 accountId = 0;
        for (uint32 i = _nextAccountIndex; i <= 1000; ++i)
        {
            uint32 id = GetOrCreateBotAccount(i);
            if (!id) continue;

            QueryResult res = CharacterDatabase.PQuery("SELECT COUNT(*) FROM characters WHERE account = %u", id);
            if (res && res->Fetch()[0].GetUInt32() < 10) // 10 bots per account
            {
                accountId = id;
                _nextAccountIndex = i; // remember: still has room, try this one again next time
                break;
            }
        }

        if (!accountId) return false;

        // 2. Generate stats
        uint8 gender = urand(0, 1) ? GENDER_MALE : GENDER_FEMALE;

        LoadNameCache();
        std::string name = GetRandomNameFromCache(gender);
        if (name.empty()) return false;

        uint8 race, cls;
        if (!GetValidRaceClassCombo(race, cls)) return false;

        // 3. Create a dummy session so Player constructor doesn't crash on RBAC checks
        std::shared_ptr<WorldSocket> nullSock;
        WorldSession* session = new WorldSession(
            accountId,
            "PSYCHOBOT_TEMPSESSION",
            0u,
            nullSock,
            SEC_PLAYER,
            uint8(CURRENT_EXPANSION),
            time_t(0),
            "Win",
            LOCALE_enUS,
            0u,
            false);

        // 4. Construct createInfo
        auto createInfo = std::make_shared<WorldPackets::Character::CharacterCreateInfo>();
        createInfo->Name = name;
        createInfo->Race = race;
        createInfo->Class = cls;
        createInfo->Sex = gender;
        createInfo->Skin = 0; // The core Player::Create will default these if invalid, or we can just pass 0.
        createInfo->Face = 0;
        createInfo->HairStyle = 0;
        createInfo->HairColor = 0;
        createInfo->FacialHairStyle = 0;
        createInfo->OutfitId = 0;

        std::shared_ptr<Player> newChar(new Player(session), [](Player* ptr)
        {
            ptr->CleanupsBeforeDelete();
            delete ptr;
        });

        newChar->GetMotionMaster()->Initialize();

        if (!newChar->Create(sObjectMgr->GetGenerator<HighGuid::Player>().Generate(), createInfo.get()))
        {
            delete session;
            return false;
        }

        newChar->setCinematic(1);
        newChar->SetAtLoginFlag(AT_LOGIN_FIRST);

        CharacterDatabaseTransaction characterTransaction = CharacterDatabase.BeginTransaction();
        LoginDatabaseTransaction trans = LoginDatabase.BeginTransaction();
        newChar->SaveToDB(trans, characterTransaction, true);
        CharacterDatabase.CommitTransaction(characterTransaction);
        LoginDatabase.CommitTransaction(trans);

        // Register the new character with the in-memory CharacterCache.
        // Without this, sCharacterCache->GetCharacterCacheByName() (used by
        // LoginBotByName / ".psychobot summon"/"add") won't find this bot
        // until the worldserver restarts and reloads the cache from DB.
        sCharacterCache->AddCharacterCacheEntry(newChar->GetGUID(), accountId, name,
            gender, race, cls, newChar->getLevel(), false);

        // Force destruction of the Player object while the session is still valid
        newChar.reset();

        // Cleanup the dummy session.
        delete session;

        TC_LOG_INFO("module.psychobot", "PsychobotFactory: Generated bot '%s' (Race: %u, Class: %u) on account %u", name.c_str(), race, cls, accountId);
        return true;
    }

    void PsychobotFactory::PurgeAllBots()
    {
        // Fetches all PSYCHOBOT_ accounts, iterates their characters, and cleanly calls Player::DeleteFromDB
        QueryResult res = LoginDatabase.Query("SELECT id FROM account WHERE username LIKE 'PSYCHOBOT_%'");
        if (!res)
        {
            TC_LOG_INFO("module.psychobot", "PsychobotFactory: No bot accounts found to purge.");
            return;
        }

        uint32 deleted = 0;
        do
        {
            uint32 accId = res->Fetch()[0].GetUInt32();
            QueryResult charRes = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE account = %u", accId);
            if (charRes)
            {
                do
                {
                    ObjectGuid::LowType guidLow = charRes->Fetch()[0].GetUInt64();
                    ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(guidLow);

                    // Native cascade delete
                    Player::DeleteFromDB(guid, accId);

                    // Clean up mod-psychobot's own per-character rows so they
                    // don't pile up as orphaned data after repeated purges.
                    CharacterDatabase.PExecute("DELETE FROM ai_playerbot_db_store WHERE guid = " UI64FMTD, guidLow);
                    CharacterDatabase.PExecute("DELETE FROM ai_playerbot_random_bots WHERE owner = " UI64FMTD " OR bot = " UI64FMTD, guidLow, guidLow);
                    CharacterDatabase.PExecute("DELETE FROM ai_playerbot_guild_tasks WHERE owner = " UI64FMTD, guidLow);

                    deleted++;
                } while (charRes->NextRow());
            }

            // Delete account
            LoginDatabase.PExecute("DELETE FROM account WHERE id = %u", accId);
            LoginDatabase.PExecute("DELETE FROM rbac_account_permissions WHERE accountId = %u", accId);

        } while (res->NextRow());

        TC_LOG_INFO("module.psychobot", "PsychobotFactory: Purged %u bot characters and their accounts.", deleted);
    }
}
