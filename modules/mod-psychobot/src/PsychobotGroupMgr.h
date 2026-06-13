/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotGroupMgr - grouping + role helpers (Stage 4) + group/raid combat
 *  coordination (S20). Lets a master pull a bot into the party, assigns the
 *  bot's LFG role from its spec, and provides group-context queries the
 *  strategies use (assist target, group leader, is-in-battleground/dungeon).
 *  S20 adds: main-assist/main-tank target priority, raid-target-icon (RTI)
 *  Skull marking, ready-check auto-accept, and a per-bot follow-formation angle.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_GROUP_MGR_H
#define PSYCHOBOT_GROUP_MGR_H

#include "Define.h"

class Player;
class Unit;
class Group;

namespace psychobot
{
    enum class SpecRole : uint8;  // defined in ai/PsychobotSpecRoles.h

    namespace BotGroupMgr
    {
        // Invite + accept the bot into the master's group (creating one if
        // needed). Returns a status string.
        bool InviteToGroup(Player* master, Player* bot);

        // Remove the bot from its current group.
        void LeaveGroup(Player* bot);

        // Set the bot's LFG role flags from its spec role (tank/heal/dps).
        void AssignRole(Player* bot);

        // Role of a bot derived from its current spec (via SpecRoles).
        SpecRole GetBotRole(Player* bot);

        // The unit a DPS/tank bot should be hitting. S20 priority order:
        //   1. the Skull (RTI 7) marked enemy, if anyone is on it
        //   2. the group's main-assist's victim (MEMBER_FLAG_MAINASSIST)
        //   3. the main-tank's victim (MEMBER_FLAG_MAINTANK) / any tank's victim
        //   4. the leader's victim
        Unit* GetGroupAssistTarget(Player* bot);

        // The most-injured friendly group member a healer bot should heal, or
        // nullptr if none need healing past the threshold. S20: prioritises the
        // main-tank when several members are hurt.
        Unit* GetGroupHealTarget(Player* bot, float belowPct = 80.0f);

        // S21: a group member (or self) carrying a debuff this bot can dispel
        // (dispelMask = bitmask of (1<<DispelType)), or nullptr. The healer
        // cures it. Self is considered too.
        Unit* GetGroupDispelTarget(Player* bot, uint32 dispelMask);

        bool InBattleground(Player* bot);
        bool InDungeon(Player* bot);

        // --- S20 group/raid coordination ------------------------------------
        // A tank/leader bot marks its current target with the Skull icon (RTI 7)
        // so the rest of the group focuses it. No-op if not in a group / no
        // target / not the designated marker. Returns true if a mark was set.
        bool MarkSkullTarget(Player* bot);

        // Auto-accept an in-progress ready check (bots are always ready).
        // Returns true if it answered one.
        bool AnswerReadyCheck(Player* bot);

        // The follow angle (radians, behind the master) for this bot, spread by
        // its slot in the group so bots fan out instead of stacking.
        float GetFollowFormationAngle(Player* bot);
    }
}

#endif // PSYCHOBOT_GROUP_MGR_H
