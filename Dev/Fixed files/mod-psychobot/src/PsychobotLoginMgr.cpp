/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotLoginMgr.h"
#include "Common.h"                 // LOCALE_enUS, SEC_PLAYER, AccountTypes
#include "SharedDefines.h"          // CURRENT_EXPANSION
#include "WorldSession.h"
#include "World.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "CharacterCache.h"
#include "AccountMgr.h"
#include "Log.h"
#include <memory>

namespace psychobot
{
    namespace LoginMgr
    {
        WorldSession* FindBotSession(uint32 accountId)
        {
            WorldSession* s = sWorld->FindSession(accountId);
            if (s && s->IsBot())
                return s;
            return nullptr;
        }

        bool HasBotSession(ObjectGuid guid)
        {
            CharacterCacheEntry const* entry = sCharacterCache->GetCharacterCacheByGuid(guid);
            if (!entry)
                return false;
            return FindBotSession(entry->AccountId) != nullptr;
        }

        bool LoginBotByName(std::string const& charName, std::string& reason)
        {
            CharacterCacheEntry const* entry = sCharacterCache->GetCharacterCacheByName(charName);
            if (!entry)
            {
                reason = "No character named '" + charName + "' exists.";
                return false;
            }
            return LoginBot(entry->Guid, entry->AccountId, reason);
        }

        bool LoginBot(ObjectGuid guid, uint32 accountId, std::string& reason)
        {
            // Already in the world as a real, connected player?
            if (ObjectAccessor::FindConnectedPlayer(guid))
            {
                reason = "That character is already online.";
                return false;
            }

            // An account can only hold one live session. If this account is
            // already running a bot, reuse nothing - refuse to double-login.
            if (sWorld->FindSession(accountId))
            {
                reason = "The owning account already has an active session.";
                return false;
            }

            // Resolve the account name (used by the session for logging only).
            std::string accountName;
            if (!AccountMgr::GetName(accountId, accountName))
                accountName = "psychobot";

            AccountTypes security = AccountTypes(AccountMgr::GetSecurity(accountId));

            // --- Construct the socketless session ----------------------------
            //   sock          = nullptr   -> socketless (the key bot trick)
            //   battlenetAcc  = 0         -> bots use no Battle.net account
            //   expansion     = CURRENT   -> match the realm's expansion (BfA=7)
            //   mute_time     = 0         -> not muted
            //   os            = "Win"     -> matches a normal client string
            //   recruiter     = 0, isARecruiter = false
            std::shared_ptr<WorldSocket> nullSock = nullptr;
            WorldSession* botSession = new WorldSession(
                accountId,
                std::string(accountName),
                0u,
                nullSock,
                security,
                uint8(CURRENT_EXPANSION),
                time_t(0),
                "Win",
                LOCALE_enUS,
                0u,
                false);

            // Mark it as a bot BEFORE adding to the world so every null-socket
            // path (queue bypass, SendPacket, IsConnectionIdle, Update) sees it.
            botSession->SetBot(true);

            // AddSession_ skips the login queue for bots and runs
            // InitializeSession() (account-data callbacks). Those callbacks and
            // the character-login callback are both serviced by
            // ProcessQueryCallbacks() inside WorldSession::Update(), with
            // account-init checked before HandlePlayerLogin in the same pass.
            sWorld->AddSession(botSession);

            // Drive the socketless character load. HandlePlayerLogin fires on a
            // later tick once the LoginQueryHolder queries complete.
            botSession->BotLogin(guid);

            TC_LOG_INFO("server.loading", "[Psychobot] Socketless login started for %s (account %u).",
                guid.ToString().c_str(), accountId);

            reason = "Logging in bot character (account " + std::to_string(accountId) + ").";
            return true;
        }

        void LogoutBot(ObjectGuid guid)
        {
            CharacterCacheEntry const* entry = sCharacterCache->GetCharacterCacheByGuid(guid);
            if (!entry)
                return;

            WorldSession* s = FindBotSession(entry->AccountId);
            if (!s)
                return;

            // LogoutPlayer(true) handles save + group/guild/map cleanup and
            // sends update packets to nearby real players (all safe - the bot's
            // own SendPacket is a guarded no-op). It is required before removal.
            if (s->GetPlayer())
                s->LogoutPlayer(true);

            // Flag the session for teardown. The next World::UpdateSessions pass
            // sees WorldSession::Update() return false (because IsBotRemoving())
            // and erases + deletes the session itself - so we must NOT delete it
            // here (that would double-free). We deliberately avoid
            // World::RemoveSession too, since its KickPlayer() is a no-op for
            // bots and would not trigger erasure.
            s->SetBotRemove();

            TC_LOG_INFO("server.loading", "[Psychobot] Socketless logout for %s (account %u).",
                guid.ToString().c_str(), entry->AccountId);
        }
    }
}
