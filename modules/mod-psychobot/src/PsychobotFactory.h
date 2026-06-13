/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_FACTORY_H
#define PSYCHOBOT_FACTORY_H

#include "Define.h"
#include <string>
#include <vector>

namespace psychobot
{
    class PsychobotFactory
    {
    public:
        static PsychobotFactory* instance();

        // Attempts to safely generate ONE random character and bot account (if needed).
        // Called sequentially to prevent server starvation/lag.
        bool GenerateOneBot();

        // Delete all PSYCHOBOT_ accounts and their characters cleanly.
        void PurgeAllBots();

    private:
        PsychobotFactory() = default;

        // Load names from ai_playerbot_names on boot
        void LoadNameCache();

        std::string GetRandomNameFromCache(uint8 gender);
        bool GetValidRaceClassCombo(uint8& outRace, uint8& outClass) const;
        uint32 GetOrCreateBotAccount(uint32 botIndex);

        std::vector<std::string> _maleNames;
        std::vector<std::string> _femaleNames;

        // Remembers the lowest PSYCHOBOT_N account index that still had
        // room for another character last time we checked, so
        // GenerateOneBot() doesn't have to rescan from account #1 every
        // single call.
        uint32 _nextAccountIndex = 1;
    };
}

#define sPsychobotFactory psychobot::PsychobotFactory::instance()

#endif // PSYCHOBOT_FACTORY_H
