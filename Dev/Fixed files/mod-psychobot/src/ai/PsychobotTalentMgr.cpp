/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotTalentMgr.h"
#include "Player.h"
#include "DB2Stores.h"
#include "DB2Structure.h"
#include "DBCEnums.h"
#include "Log.h"
#include <vector>

namespace psychobot
{
    namespace TalentMgr
    {
        uint32 EnsureSpecialization(Player* bot, int32 specIndex)
        {
            if (!bot)
                return 0;

            // Already has a primary spec? keep it.
            if (uint32 cur = bot->GetPrimarySpecialization())
                return cur;

            uint32 cls = bot->getClass();

            ChrSpecializationEntry const* spec = nullptr;
            if (specIndex >= 0)
                spec = sDB2Manager.GetChrSpecializationByIndex(cls, static_cast<uint32>(specIndex));
            if (!spec)
                spec = sDB2Manager.GetDefaultChrSpecializationForClass(cls);
            if (!spec)
                return 0;

            bot->SetPrimarySpecialization(spec->ID);
            bot->ActivateTalentGroup(spec);
            return spec->ID;
        }

        void ApplyTalents(Player* bot, uint8 preferColumn)
        {
            if (!bot)
                return;

            uint32 cls = bot->getClass();
            uint8  level = bot->getLevel();

            // BfA tiers unlock at these levels (row 0..6).
            static uint8 const TIER_UNLOCK_LEVEL[MAX_TALENT_TIERS] =
                { 15, 30, 45, 60, 75, 90, 100 };

            for (uint32 tier = 0; tier < MAX_TALENT_TIERS; ++tier)
            {
                if (level < TIER_UNLOCK_LEVEL[tier])
                    break; // higher tiers not yet available

                // Try the preferred column first, then fall back across columns.
                bool learned = false;
                for (uint32 step = 0; step < MAX_TALENT_COLUMNS && !learned; ++step)
                {
                    uint32 column = (preferColumn + step) % MAX_TALENT_COLUMNS;
                    std::vector<TalentEntry const*> const& talents =
                        sDB2Manager.GetTalentsByPosition(cls, tier, column);

                    for (TalentEntry const* talent : talents)
                    {
                        if (!talent)
                            continue;
                        int32 onCd = 0;
                        if (bot->LearnTalent(talent->ID, &onCd) == TALENT_LEARN_OK)
                        {
                            learned = true;
                            break;
                        }
                    }
                }
            }
        }

        void SetupSpec(Player* bot, int32 specIndex, uint8 preferColumn)
        {
            if (!bot)
                return;
            EnsureSpecialization(bot, specIndex);
            ApplyTalents(bot, preferColumn);
        }
    }
}
