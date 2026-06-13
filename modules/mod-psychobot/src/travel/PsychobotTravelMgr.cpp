/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotTravelMgr.h"
#include "Player.h"
#include "Unit.h"
#include "MotionMaster.h"
#include <map>
#include <cmath>

namespace psychobot
{
    namespace TravelMgr
    {
        WorldPosition GetPosition(Player* bot)
        {
            if (!bot || !bot->IsInWorld())
                return WorldPosition();
            return WorldPosition(bot->GetMapId(), bot->GetPositionX(),
                                 bot->GetPositionY(), bot->GetPositionZ(),
                                 bot->GetOrientation());
        }

        float DistanceTo(Player* bot, WorldPosition const& dest)
        {
            if (!bot || !bot->IsInWorld() || bot->GetMapId() != dest.mapId)
                return 99999.0f;
            float dx = bot->GetPositionX() - dest.x;
            float dy = bot->GetPositionY() - dest.y;
            float dz = bot->GetPositionZ() - dest.z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        bool MoveTo(Player* bot, WorldPosition const& dest, float walkRange, float teleportRange)
        {
            if (!bot || !bot->IsInWorld() || !dest.IsValid())
                return false;

            // Cross-map -> teleport (no pathing possible).
            if (bot->GetMapId() != dest.mapId)
            {
                bot->TeleportTo(dest.mapId, dest.x, dest.y, dest.z, dest.o);
                return true;
            }

            float dist = DistanceTo(bot, dest);

            // Too far to path sensibly -> teleport to catch up.
            if (dist > teleportRange)
            {
                bot->TeleportTo(dest.mapId, dest.x, dest.y, dest.z, dest.o);
                return true;
            }

            // Within walking range -> pathed point move.
            if (dist > 1.0f && dist <= walkRange + 0.01f)
            {
                bot->GetMotionMaster()->MovePoint(0, dest.x, dest.y, dest.z, /*generatePath*/ true);
                return true;
            }

            // Between walkRange and teleportRange: still path toward it (the
            // pathfinder will route as far as it can).
            if (dist > walkRange)
            {
                bot->GetMotionMaster()->MovePoint(0, dest.x, dest.y, dest.z, true);
                return true;
            }
            return false;   // already there
        }

        bool FollowMasterTravel(Player* bot, Player* master, float followDist)
        {
            if (!bot || !master || !bot->IsInWorld() || !master->IsInWorld())
                return false;

            // Different map, or far away -> teleport to the master's spot.
            if (bot->GetMapId() != master->GetMapId())
            {
                bot->TeleportTo(master->GetMapId(), master->GetPositionX(),
                                master->GetPositionY(), master->GetPositionZ(),
                                master->GetOrientation());
                return true;
            }

            float dist = bot->GetDistance(master);

            // Beyond the leash -> teleport in (covers loading-screen gaps, etc.).
            float const leash = 80.0f;
            if (dist > leash)
            {
                bot->TeleportTo(master->GetMapId(), master->GetPositionX(),
                                master->GetPositionY(), master->GetPositionZ(),
                                master->GetOrientation());
                return true;
            }

            // Otherwise use the normal follow generator (with the formation
            // angle the caller already applies). Returning false lets the AI's
            // own MoveFollow run; we only own the catch-up here.
            (void)followDist;
            return false;
        }

        // --- seed travel-node registry --------------------------------------
        namespace
        {
            std::map<std::string, TravelNode>& Nodes()
            {
                static std::map<std::string, TravelNode> nodes;
                return nodes;
            }
        }

        void AddNode(std::string const& name, WorldPosition const& pos)
        {
            TravelNode node;
            node.name = name;
            node.pos = pos;
            Nodes()[name] = node;
        }

        TravelNode const* GetNode(std::string const& name)
        {
            auto it = Nodes().find(name);
            return it != Nodes().end() ? &it->second : nullptr;
        }
    }
}
