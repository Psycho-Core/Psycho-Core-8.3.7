/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotAhBot.h"
#include "Config.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Entities/Item/ItemTemplate.h"

namespace psychobot
{
    PsychobotAhBot* PsychobotAhBot::instance()
    {
        static PsychobotAhBot instance;
        return &instance;
    }

    void PsychobotAhBot::LoadConfig()
    {
        _enable             = sConfigMgr->GetBoolDefault ("Psychobot.AhBot.Enable", false);
        _intervalSeconds    = sConfigMgr->GetIntDefault  ("Psychobot.AhBot.IntervalSeconds", 3600);
        _maxListingsPerCycle= sConfigMgr->GetIntDefault  ("Psychobot.AhBot.MaxListingsPerCycle", 20);
        _minItemLevel       = sConfigMgr->GetIntDefault  ("Psychobot.AhBot.MinItemLevel", 1);
        _maxQuality         = sConfigMgr->GetIntDefault  ("Psychobot.AhBot.MaxQuality", 4);   // Epic
        _priceMultiplier    = sConfigMgr->GetFloatDefault("Psychobot.AhBot.PriceMultiplier", 1.5f);
        if (_priceMultiplier < 1.0f)
            _priceMultiplier = 1.0f;
    }

    // ----------------------------------------------------------------------
    // Candidate selection - what may the ahbot sell?
    // ----------------------------------------------------------------------
    bool PsychobotAhBot::IsSellable(uint32 itemEntry) const
    {
        ItemTemplate const* t = sObjectMgr->GetItemTemplate(itemEntry);
        if (!t)
            return false;

        // Skip non-tradable categories.
        uint32 cls = t->GetClass();
        if (cls == ITEM_CLASS_QUEST || cls == ITEM_CLASS_KEY)
            return false;

        // Skip soulbound / quest-bound items (can't be auctioned).
        ItemBondingType bond = t->GetBonding();
        if (bond == BIND_ON_ACQUIRE || bond == BIND_QUEST)
            return false;

        // Quality + a real merchant value gate (no 0-value vendor trash spam).
        if (t->GetQuality() > _maxQuality)
            return false;
        if (t->GetSellPrice() == 0)
            return false;

        // Required-level floor maps roughly to "item level" for our purposes.
        if (uint32(t->GetBaseRequiredLevel()) < _minItemLevel && t->GetBaseRequiredLevel() != 0)
            return false;

        return true;
    }

    // ----------------------------------------------------------------------
    // Price model - quality-aware markup over the base merchant sell price.
    // ----------------------------------------------------------------------
    uint64 PsychobotAhBot::PriceItem(uint32 itemEntry) const
    {
        ItemTemplate const* t = sObjectMgr->GetItemTemplate(itemEntry);
        if (!t)
            return 0;

        uint64 base = t->GetSellPrice();
        if (base == 0)
            base = 1;

        // Quality bumps the markup: higher rarity = higher AH premium.
        float qualityFactor = 1.0f + 0.25f * float(t->GetQuality());   // white 1.0 .. epic 2.0
        uint64 price = uint64(double(base) * double(_priceMultiplier) * double(qualityFactor));
        return price ? price : 1;
    }

    // ----------------------------------------------------------------------
    // Build the cycle's listings (pure / deterministic - no AH access).
    // ----------------------------------------------------------------------
    std::vector<AhListing> PsychobotAhBot::BuildListings() const
    {
        std::vector<AhListing> out;

        ItemTemplateContainer const* store = sObjectMgr->GetItemTemplateStore();
        if (!store)
            return out;

        for (auto const& pair : *store)
        {
            if (out.size() >= _maxListingsPerCycle)
                break;

            uint32 entry = pair.first;
            if (!IsSellable(entry))
                continue;

            ItemTemplate const& t = pair.second;

            AhListing listing;
            listing.itemEntry  = entry;
            // Stack size: full stack for stackables, single otherwise.
            uint32 maxStack = t.GetMaxStackSize();
            listing.stackCount = maxStack > 1 ? (maxStack > 20 ? 20 : maxStack) : 1;
            listing.auctions   = 1;
            listing.unitPrice  = PriceItem(entry);
            out.push_back(listing);
        }
        return out;
    }

    // ----------------------------------------------------------------------
    // Tick + cycle
    // ----------------------------------------------------------------------
    void PsychobotAhBot::Update(uint32 diff)
    {
        if (!_enable)
            return;

        _accum += diff;
        if (_accum < _intervalSeconds * 1000)
            return;
        _accum = 0;

        RunCycle();
    }

    void PsychobotAhBot::RunCycle()
    {
        std::vector<AhListing> listings = BuildListings();
        uint32 posted = 0;
        for (AhListing const& listing : listings)
            if (PostAuction(listing))
                ++posted;

        TC_LOG_INFO("module.psychobot", "AhBot cycle: %u candidate listings, %u posted.",
            uint32(listings.size()), posted);
    }

    bool PsychobotAhBot::PostAuction(AhListing const& /*listing*/)
    {
        // The one irreducibly-live step: create the Item* and call the core
        // AuctionHouseObject::AddAuction under an AH-owner account. This needs a
        // configured owner account + the client item DB2 loaded, and can't be
        // exercised without a running server, so it is a documented hook here.
        // BuildListings()/PriceItem()/IsSellable() above ARE the full ahbot
        // decision logic and run/validate without the AH.
        //
        // On-server completion (per the reference ahbot/ seller flow):
        //   1. Item* item = Item::CreateItem(listing.itemEntry, listing.stackCount, ...);
        //   2. AuctionPosting auction; fill Owner/OwnerAccount/BuyoutOrUnitPrice/
        //      Deposit/StartTime/EndTime/Items.push_back(item);
        //   3. sAuctionMgr->GetAuctionsById(houseId)->AddAuction(trans, auction);
        return false;
    }
}
