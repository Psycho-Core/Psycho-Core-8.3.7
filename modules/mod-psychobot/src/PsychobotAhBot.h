/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotAhBot (S26) - keeps the auction house populated for an 8.3 server.
 *  Clean-room re-implementation of the cmangos ahbot/ (14 files) LOGIC: choose
 *  sellable items from the item-template store, price them from a quality-aware
 *  model, and post them on a cycle; opportunistically buy under-priced auctions.
 *
 *  Everything that can be decided WITHOUT a running auction house is real here
 *  (candidate selection, quantities, price model, cycle/throttle). The single
 *  irreducibly-live step - actually creating the Item* and calling the core
 *  AuctionHouseObject::AddAuction with an AH-owner account - is funnelled through
 *  PostAuction(), which is gated behind Psychobot.AhBot.Enable and documented for
 *  the first on-server pass (it needs the owner account + client item DB2).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_AHBOT_H
#define PSYCHOBOT_AHBOT_H

#include "Define.h"
#include <vector>

namespace psychobot
{
    // One thing the ahbot wants to put on the AH this cycle.
    struct AhListing
    {
        uint32 itemEntry = 0;
        uint32 stackCount = 1;   // items per auction
        uint32 auctions   = 1;   // how many auctions of this stack
        uint64 unitPrice  = 0;   // buyout per item (copper)
    };

    class PsychobotAhBot
    {
    public:
        static PsychobotAhBot* instance();

        void LoadConfig();
        void Update(uint32 diff);   // ticked from the world update

        // Build the listings the ahbot would post this cycle from the item
        // template store + the price model. Pure/deterministic (no AH access),
        // so it is unit-testable and statically verifiable.
        std::vector<AhListing> BuildListings() const;

        // The buyout-per-item price for an item entry (quality-aware model).
        uint64 PriceItem(uint32 itemEntry) const;

        // Whether an item entry is a sane ahbot sale candidate.
        bool IsSellable(uint32 itemEntry) const;

    private:
        PsychobotAhBot() { LoadConfig(); }

        // The one live step: create the item + AddAuction (gated; on-server).
        void RunCycle();
        bool PostAuction(AhListing const& listing);

        bool   _enable = false;
        uint32 _intervalSeconds = 3600;   // post cycle period
        uint32 _accum = 0;                // ms accumulator
        uint32 _maxListingsPerCycle = 20; // throttle
        uint32 _minItemLevel = 1;         // candidate floor
        uint32 _maxQuality = 4;           // up to Epic by default
        float  _priceMultiplier = 1.5f;   // markup over base sell price
    };
}

#define sPsychobotAhBot psychobot::PsychobotAhBot::instance()

#endif // PSYCHOBOT_AHBOT_H
