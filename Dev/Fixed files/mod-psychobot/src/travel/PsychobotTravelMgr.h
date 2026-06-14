/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Travel system (S23) - MINIMAL start: local navigation + robust follow.
 *    WorldPosition - lightweight {mapId, x, y, z, o}. (The reference's heavy
 *                    WorldPosition can be grown into here later.)
 *    TravelNode    - a named WorldPosition; a registry of nodes is the seed of
 *                    the 8.3 travel graph (expanded per-map in a later pass).
 *    TravelMgr     - MoveTo (local MovePoint with pathing, or TeleportTo when
 *                    cross-map / beyond the leash) and FollowMasterTravel (the
 *                    distance/cross-map-aware catch-up the AI calls each tick).
 *
 *  This is intentionally small: it makes "follow" work across distance and maps
 *  without the full node-graph navigation, which we layer on later.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_TRAVEL_MGR_H
#define PSYCHOBOT_TRAVEL_MGR_H

#include "Define.h"
#include <string>

class Player;
class Unit;

namespace psychobot
{
    // A point in the world: map + coordinates + facing.
    struct WorldPosition
    {
        uint32 mapId = 0;
        float  x = 0.0f;
        float  y = 0.0f;
        float  z = 0.0f;
        float  o = 0.0f;

        WorldPosition() = default;
        WorldPosition(uint32 m, float px, float py, float pz, float po = 0.0f)
            : mapId(m), x(px), y(py), z(pz), o(po) { }

        bool IsValid() const { return mapId != 0 || x != 0.0f || y != 0.0f || z != 0.0f; }
    };

    // A named node in the (seed) travel graph.
    struct TravelNode
    {
        std::string   name;
        WorldPosition pos;
    };

    namespace TravelMgr
    {
        // The bot's current position.
        WorldPosition GetPosition(Player* bot);

        // Distance from the bot to a position on the SAME map (huge if maps differ).
        float DistanceTo(Player* bot, WorldPosition const& dest);

        // Move the bot toward `dest`. Same map + within `walkRange`: pathed
        // MovePoint. Cross-map, or farther than `teleportRange`: TeleportTo.
        // Returns true if a move/teleport was issued.
        bool MoveTo(Player* bot, WorldPosition const& dest,
                    float walkRange = 60.0f, float teleportRange = 100.0f);

        // Distance/cross-map-aware follow: stay near the master, pathing when
        // close and teleporting to catch up when far or on another map. The AI's
        // FollowMaster routes here. `followDist` is the desired stand-off.
        bool FollowMasterTravel(Player* bot, Player* master, float followDist = 2.0f);

        // --- seed travel-node registry (grown for 8.3 maps later) ------------
        void              AddNode(std::string const& name, WorldPosition const& pos);
        TravelNode const* GetNode(std::string const& name);
    }
}

#endif // PSYCHOBOT_TRAVEL_MGR_H
