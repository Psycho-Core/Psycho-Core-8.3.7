/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotLoginMgr (S28) - socketless bot login/logout. Creates a real
 *  WorldSession with no WorldSocket (m_isBot = true), adds it to the world,
 *  and drives WorldSession::BotLogin(guid) so the core's own async login path
 *  (LoginQueryHolder -> HandlePlayerLogin via ProcessQueryCallbacks) brings an
 *  OFFLINE character fully into the world as a bot. Teardown calls LogoutPlayer
 *  + RemoveSession so group/guild/map cleanup runs exactly as for a real player.
 *
 *  This is the only Psychobot feature that depends on the seven S28 core edits
 *  (WorldSession m_isBot / IsBot / SetBot / BotLogin + the null-socket guards).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_LOGIN_MGR_H
#define PSYCHOBOT_LOGIN_MGR_H

#include "Define.h"
#include "ObjectGuid.h"
#include <string>

class WorldSession;

namespace psychobot
{
    namespace LoginMgr
    {
        // Resolve an OFFLINE character by name and log it in socketlessly as a
        // bot. Returns true on success (the character then finishes loading on
        // a later world tick); 'reason' carries a human-readable status.
        bool LoginBotByName(std::string const& charName, std::string& reason);

        // Same, by character GUID + the owning account id (skips the name
        // lookup). accountId must be the account that owns 'guid'.
        bool LoginBot(ObjectGuid guid, uint32 accountId, std::string& reason);

        // Cleanly log a bot character out of the world and drop its bot
        // session. Safe to call with a guid that isn't a bot (no-op).
        void LogoutBot(ObjectGuid guid);

        // True if 'guid' currently has a live bot WorldSession in the world.
        bool HasBotSession(ObjectGuid guid);

        // The bot WorldSession for an account id, or nullptr.
        WorldSession* FindBotSession(uint32 accountId);
    }
}

#endif // PSYCHOBOT_LOGIN_MGR_H
