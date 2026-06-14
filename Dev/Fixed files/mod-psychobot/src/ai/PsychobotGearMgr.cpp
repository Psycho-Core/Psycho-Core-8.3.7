/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotGearMgr.h"
#include "PsychobotPopulationMgr.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "ItemTemplate.h"
#include "ItemDefines.h"
#include "Log.h"

namespace psychobot
{
    namespace GearMgr
    {
        bool IsAcceptableItem(Player* bot, uint32 itemId, uint32 qualityCap)
        {
            if (!bot || !itemId)
                return false;

            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId);
            if (!proto)
                return false;

            // Quality cap (e.g. 3 = rare). 0 = no cap.
            if (qualityCap && proto->GetQuality() > qualityCap)
                return false;

            // Level appropriateness: don't hand out gear above the bot's level.
            int32 reqLevel = proto->GetBaseRequiredLevel();
            if (reqLevel > int32(bot->getLevel()))
                return false;

            // Usability: class/armor proficiency, etc. (the authoritative check).
            if (bot->CanUseItem(proto) != EQUIP_ERR_OK)
                return false;

            return true;
        }

        void GearUp(Player* bot)
        {
            if (!bot)
                return;

            PopulationConfig const& cfg = sPsychobotPopulation->Config();

            // EquipmentPersistence: stop re-rolling gear at/above the configured
            // level (0 means "at max level" -> never persist before max).
            if (cfg.equipmentPersistence)
            {
                uint32 stopLevel = cfg.equipmentPersistenceLevel
                    ? cfg.equipmentPersistenceLevel : cfg.randomBotMaxLevel;
                if (bot->getLevel() >= stopLevel)
                    return; // gear considered finalized; leave it alone
            }

            // Framework: the actual per-slot item selection requires the client
            // item DB2 to enumerate level/quality/armor-type appropriate items.
            // When that data is present, iterate candidate items, filter via
            // IsAcceptableItem(..., cfg.gearQualityLimit) (and preferClassArmorType),
            // and StoreNewItemInBestSlots() the best per slot.
            //
            // Until item data is available this is a safe no-op so the build and
            // runtime are stable. (Marked for completion alongside client data.)
            EnsureBasics(bot);
        }

        void EnsureBasics(Player* /*bot*/)
        {
            // Stub-safe: ensure bags/food/drink once item DB2 is available.
            // No-op for now (no hardcoded item IDs without client data).
        }
    }
}
