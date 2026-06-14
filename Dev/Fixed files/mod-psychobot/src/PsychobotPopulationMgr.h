/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotPopulationMgr - open-world random-bot population + the scaling
 *  system (BotActiveAlone / SmartScale / DisabledWithoutRealPlayer), ported in
 *  design from the user's Mangos-Bots Cataclysm patch.
 *
 *  S30 NOTE: the random-bot deficit hook is now wired to PsychobotFactory,
 *  which can generate PSYCHOBOT accounts/characters and feed them back into
 *  the socketless login path one at a time without blocking the world tick.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_POPULATION_MGR_H
#define PSYCHOBOT_POPULATION_MGR_H

#include "Define.h"
#include "ObjectGuid.h"
#include <set>
#include <ctime>

class Player;

namespace psychobot
{
    // Tunables, loaded from config (mirrors the user's Cata-Patch keys).
    struct PopulationConfig
    {
        bool   enable                 = false;
        uint32 minRandomBots          = 0;
        uint32 maxRandomBots          = 0;
        uint32 randomBotMinLevel      = 1;
        uint32 randomBotMaxLevel      = 120;   // BfA cap
        bool   syncLevelWithPlayers   = false;

        // BotActiveAlone / SmartScale
        uint32 botActiveAlone         = 100;   // % active when alone
        uint32 activeAloneDuration    = 120;   // roster rotation seconds
        float  forceActiveRadius      = 200.0f;
        bool   smartScale             = true;
        uint32 smartScaleDiffFloor    = 50;    // ms
        uint32 smartScaleDiffCeiling  = 150;   // ms

        // DisabledWithoutRealPlayer
        bool   disabledWithoutRealPlayer = true;
        uint32 loginDelaySeconds         = 0;
        uint32 logoutDelaySeconds        = 0;

        // Gear
        uint32 gearQualityLimit       = 3;     // ITEM_QUALITY_RARE
        bool   preferClassArmorType   = true;
        bool   equipmentPersistence   = true;
        uint32 equipmentPersistenceLevel = 0;  // 0 = max level
    };

    class PsychobotPopulationMgr
    {
    public:
        static PsychobotPopulationMgr* instance();

        void LoadConfig();                 // (re)read config keys
        PopulationConfig const& Config() const { return _cfg; }

        // World tick (called from PsychobotMgr::UpdateAI). Drives population +
        // roster rotation + smart-scale decisions. Returns the effective active
        // percentage in use this tick (for diagnostics).
        void Update(uint32 diff);

        // Scaling query used by PsychobotAI: should this bot tick its AI now?
        bool ShouldBotBeActive(Player* bot) const;

        // Are there any real (non-bot) players online right now?
        bool HasRealPlayerOnline() const;

        // Roster: bots currently selected to be active when "alone".
        bool IsInActiveRoster(ObjectGuid guid) const;

    private:
        PsychobotPopulationMgr() { LoadConfig(); }

        void RotateActiveRoster();
        bool IsRealPlayerNear(Player* bot) const;
        uint32 EffectiveActivePercent() const;   // applies SmartScale to botActiveAlone

        PopulationConfig _cfg;
        std::set<ObjectGuid> _activeRoster;
        time_t  _lastRosterRotation = 0;
        time_t  _lastRealPlayerSeen = 0;
        uint32  _lastWorldDiff = 0;              // last tick ms (for SmartScale)
        uint32  _rosterAccum = 0;                // ms accumulator
    };
}

#define sPsychobotPopulation psychobot::PsychobotPopulationMgr::instance()

#endif // PSYCHOBOT_POPULATION_MGR_H
