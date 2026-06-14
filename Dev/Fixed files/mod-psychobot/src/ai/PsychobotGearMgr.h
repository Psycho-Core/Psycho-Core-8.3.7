/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotGearMgr - level-appropriate gear/inventory for bots. Filters by
 *  usability (class/armor proficiency via CanUseItem), quality cap and level;
 *  honours preferClassArmorType + equipmentPersistence. Exact item selection
 *  depends on the client item DB2 - the logic is complete and degrades safely
 *  when item data is unavailable.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_GEAR_MGR_H
#define PSYCHOBOT_GEAR_MGR_H

#include "Define.h"

class Player;

namespace psychobot
{
    namespace GearMgr
    {
        // Gear a bot to roughly its level. Respects quality cap / persistence
        // config. Safe to call repeatedly; no-op if persistence says "done".
        void GearUp(Player* bot);

        // True if a bot may equip the given item id (usable + within quality cap).
        bool IsAcceptableItem(Player* bot, uint32 itemId, uint32 qualityCap);

        // Ensure the bot has basic consumables/bags (stub-safe).
        void EnsureBasics(Player* bot);
    }
}

#endif // PSYCHOBOT_GEAR_MGR_H
