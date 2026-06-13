/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotGroupMgr.h"
#include "ai/PsychobotSpecRoles.h"
#include "engine/ServerFacade.h"
#include "Player.h"
#include "Unit.h"
#include "Group.h"
#include "GroupMgr.h"
#include "ObjectAccessor.h"
#include "DB2Stores.h"
#include "DB2Structure.h"
#include "DungeonFinding/LFG.h"
#include "Log.h"

namespace psychobot
{
    namespace BotGroupMgr
    {
        bool InviteToGroup(Player* master, Player* bot)
        {
            if (!master || !bot || master == bot)
                return false;

            Group* group = master->GetGroup();
            if (!group)
            {
                // Create a new group led by the master.
                group = new Group();
                if (!group->Create(master))
                {
                    delete group;
                    return false;
                }
                sGroupMgr->AddGroup(group);
            }

            if (group->IsFull())
                return false;
            if (bot->GetGroup())
                return false; // already grouped elsewhere

            if (!group->AddMember(bot))
                return false;

            AssignRole(bot);
            return true;
        }

        void LeaveGroup(Player* bot)
        {
            if (!bot)
                return;
            if (Group* group = bot->GetGroup())
                group->RemoveMember(bot->GetGUID());
        }

        SpecRole GetBotRole(Player* bot)
        {
            return SpecRoles::GetBotRole(bot);
        }

        void AssignRole(Player* bot)
        {
            if (!bot)
                return;
            Group* group = bot->GetGroup();
            if (!group)
                return;

            uint8 roleFlags = lfg::PLAYER_ROLE_DAMAGE;
            switch (GetBotRole(bot))
            {
                case SpecRole::Tank:   roleFlags = lfg::PLAYER_ROLE_TANK;   break;
                case SpecRole::Healer: roleFlags = lfg::PLAYER_ROLE_HEALER; break;
                default:               roleFlags = lfg::PLAYER_ROLE_DAMAGE; break;
            }
            group->SetLfgRoles(bot->GetGUID(), roleFlags);
        }

        Unit* GetGroupAssistTarget(Player* bot)
        {
            if (!bot)
                return nullptr;

            Group* group = bot->GetGroup();
            if (!group)
                return nullptr;

            // S20 priority: main-assist's victim > main-tank's victim > any
            // tank's victim > leader's victim. (We can set the Skull mark via
            // MarkSkullTarget but the core Group API has no public getter for
            // the marked GUID, so "assist the marker" is realised by everyone
            // assisting the main-assist/tank, who is the one marking + tanking.)
            Unit* assistTarget = nullptr;   // MAINASSIST flagged member's victim
            Unit* mainTankTarget = nullptr; // MAINTANK flagged member's victim
            Unit* tankTarget = nullptr;     // any tank-role member's victim
            Unit* leaderTarget = nullptr;
            ObjectGuid leaderGuid = group->GetLeaderGUID();

            for (Group::MemberSlot const& slot : group->GetMemberSlots())
            {
                Player* member = ObjectAccessor::GetPlayer(*bot, slot.guid);
                if (!member || member == bot || !member->IsInWorld() || !member->IsInMap(bot))
                    continue;

                Unit* victim = member->GetVictim();
                if (!victim)
                    continue;

                uint8 flags = group->GetMemberFlags(member->GetGUID());
                if ((flags & MEMBER_FLAG_MAINASSIST) && !assistTarget)
                    assistTarget = victim;
                if ((flags & MEMBER_FLAG_MAINTANK) && !mainTankTarget)
                    mainTankTarget = victim;
                if (member->GetGUID() == leaderGuid && !leaderTarget)
                    leaderTarget = victim;
                if (GetBotRole(member) == SpecRole::Tank && !tankTarget)
                    tankTarget = victim;
            }

            if (assistTarget)   return assistTarget;
            if (mainTankTarget) return mainTankTarget;
            if (tankTarget)     return tankTarget;
            return leaderTarget;
        }

        Unit* GetGroupHealTarget(Player* bot, float belowPct)
        {
            if (!bot)
                return nullptr;
            Group* group = bot->GetGroup();
            if (!group)
            {
                // Solo healer: heal self if hurt.
                return (bot->GetHealthPct() < belowPct) ? bot : nullptr;
            }

            Unit* worst = nullptr;
            float worstPct = belowPct;

            // Consider self.
            if (bot->GetHealthPct() < worstPct)
            {
                worst = bot;
                worstPct = bot->GetHealthPct();
            }

            // S20: a hurt main-tank gets a healing bias (tank deaths wipe groups),
            // so we discount their effective health a little when comparing.
            for (Group::MemberSlot const& slot : group->GetMemberSlots())
            {
                Player* member = ObjectAccessor::GetPlayer(*bot, slot.guid);
                if (!member || !member->IsInWorld() || !member->IsAlive() || !member->IsInMap(bot))
                    continue;

                float pct = member->GetHealthPct();
                bool isMainTank = (group->GetMemberFlags(member->GetGUID()) & MEMBER_FLAG_MAINTANK) != 0;
                float effective = isMainTank ? pct - 10.0f : pct;   // tank priority

                if (effective < worstPct)
                {
                    worst = member;
                    worstPct = effective;
                }
            }
            return worst;
        }

        Unit* GetGroupDispelTarget(Player* bot, uint32 dispelMask)
        {
            if (!bot || !dispelMask)
                return nullptr;

            // Self first (cheap, always in range).
            if (ServerFacade::HasDispellableAura(bot, dispelMask))
                return bot;

            Group* group = bot->GetGroup();
            if (!group)
                return nullptr;

            for (Group::MemberSlot const& slot : group->GetMemberSlots())
            {
                Player* member = ObjectAccessor::GetPlayer(*bot, slot.guid);
                if (!member || member == bot || !member->IsInWorld() || !member->IsAlive() || !member->IsInMap(bot))
                    continue;
                if (ServerFacade::HasDispellableAura(member, dispelMask))
                    return member;
            }
            return nullptr;
        }

        bool InBattleground(Player* bot)
        {
            return bot && bot->InBattleground();
        }

        bool InDungeon(Player* bot)
        {
            if (!bot)
                return false;
            if (Group* group = bot->GetGroup())
                return group->isLFGGroup();
            return false;
        }

        // --- S20 group/raid coordination ------------------------------------
        bool MarkSkullTarget(Player* bot)
        {
            if (!bot)
                return false;
            Group* group = bot->GetGroup();
            if (!group)
                return false;

            // Only the designated marker marks: the group leader, the main tank,
            // or (for a leaderless bot party) any tank-role bot.
            ObjectGuid guid = bot->GetGUID();
            bool isLeader   = group->GetLeaderGUID() == guid;
            bool isMainTank = (group->GetMemberFlags(guid) & MEMBER_FLAG_MAINTANK) != 0;
            if (!isLeader && !isMainTank && GetBotRole(bot) != SpecRole::Tank)
                return false;

            Unit* target = bot->GetVictim();
            if (!target)
                return false;

            // Skull = raid target icon index 7. partyIndex 0 = the main party.
            group->SetTargetIcon(7 /*Skull*/, target->GetGUID(), guid, 0);
            return true;
        }

        bool AnswerReadyCheck(Player* bot)
        {
            if (!bot)
                return false;
            Group* group = bot->GetGroup();
            if (!group || !group->IsReadyCheckStarted())
                return false;

            // Bots are always ready.
            group->SetMemberReadyCheck(bot->GetGUID(), true);
            return true;
        }

        float GetFollowFormationAngle(Player* bot)
        {
            // Default: directly behind the master (PI radians).
            float const behind = 3.14159265f;
            if (!bot)
                return behind;

            Group* group = bot->GetGroup();
            if (!group)
                return behind;

            // Spread bots by their ordinal slot in the group so they fan out in
            // an arc behind the master rather than stacking on one spot. Step ~
            // 0.35 rad per bot, alternating sides around "behind".
            int index = 0;
            for (Group::MemberSlot const& slot : group->GetMemberSlots())
            {
                if (slot.guid == bot->GetGUID())
                    break;
                ++index;
            }

            float const step = 0.35f;
            float offset = step * ((index + 1) / 2);   // 0, .35, .70, ...
            if (index % 2 == 0)
                offset = -offset;                       // alternate left/right
            return behind + offset;
        }
    }
}
