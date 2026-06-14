/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotSpecRoles.h"
#include "Player.h"
#include "DB2Stores.h"
#include "DB2Structure.h"
#include "SharedDefines.h"   // CLASS_* enum

namespace psychobot
{
    namespace
    {
        struct SpecInfo
        {
            uint8       classId;
            uint8       specIndex;   // ChrSpecialization OrderIndex (0..n)
            SpecRole    role;
            char const* name;
        };

        // BfA 8.3 class/spec -> role matrix (36 specs across 12 classes).
        SpecInfo const kSpecs[] =
        {
            { CLASS_WARRIOR,      0, SpecRole::Dps,    "Arms" },
            { CLASS_WARRIOR,      1, SpecRole::Dps,    "Fury" },
            { CLASS_WARRIOR,      2, SpecRole::Tank,   "Protection" },

            { CLASS_PALADIN,      0, SpecRole::Healer, "Holy" },
            { CLASS_PALADIN,      1, SpecRole::Tank,   "Protection" },
            { CLASS_PALADIN,      2, SpecRole::Dps,    "Retribution" },

            { CLASS_HUNTER,       0, SpecRole::Dps,    "Beast Mastery" },
            { CLASS_HUNTER,       1, SpecRole::Dps,    "Marksmanship" },
            { CLASS_HUNTER,       2, SpecRole::Dps,    "Survival" },

            { CLASS_ROGUE,        0, SpecRole::Dps,    "Assassination" },
            { CLASS_ROGUE,        1, SpecRole::Dps,    "Outlaw" },
            { CLASS_ROGUE,        2, SpecRole::Dps,    "Subtlety" },

            { CLASS_PRIEST,       0, SpecRole::Healer, "Discipline" },
            { CLASS_PRIEST,       1, SpecRole::Healer, "Holy" },
            { CLASS_PRIEST,       2, SpecRole::Dps,    "Shadow" },

            { CLASS_DEATH_KNIGHT, 0, SpecRole::Tank,   "Blood" },
            { CLASS_DEATH_KNIGHT, 1, SpecRole::Dps,    "Frost" },
            { CLASS_DEATH_KNIGHT, 2, SpecRole::Dps,    "Unholy" },

            { CLASS_SHAMAN,       0, SpecRole::Dps,    "Elemental" },
            { CLASS_SHAMAN,       1, SpecRole::Dps,    "Enhancement" },
            { CLASS_SHAMAN,       2, SpecRole::Healer, "Restoration" },

            { CLASS_MAGE,         0, SpecRole::Dps,    "Arcane" },
            { CLASS_MAGE,         1, SpecRole::Dps,    "Fire" },
            { CLASS_MAGE,         2, SpecRole::Dps,    "Frost" },

            { CLASS_WARLOCK,      0, SpecRole::Dps,    "Affliction" },
            { CLASS_WARLOCK,      1, SpecRole::Dps,    "Demonology" },
            { CLASS_WARLOCK,      2, SpecRole::Dps,    "Destruction" },

            { CLASS_MONK,         0, SpecRole::Tank,   "Brewmaster" },
            { CLASS_MONK,         1, SpecRole::Healer, "Mistweaver" },
            { CLASS_MONK,         2, SpecRole::Dps,    "Windwalker" },

            { CLASS_DRUID,        0, SpecRole::Dps,    "Balance" },
            { CLASS_DRUID,        1, SpecRole::Dps,    "Feral" },
            { CLASS_DRUID,        2, SpecRole::Tank,   "Guardian" },
            { CLASS_DRUID,        3, SpecRole::Healer, "Restoration" },

            { CLASS_DEMON_HUNTER, 0, SpecRole::Dps,    "Havoc" },
            { CLASS_DEMON_HUNTER, 1, SpecRole::Tank,   "Vengeance" },
        };

        SpecInfo const* Find(uint8 classId, uint8 specIndex)
        {
            for (SpecInfo const& s : kSpecs)
                if (s.classId == classId && s.specIndex == specIndex)
                    return &s;
            return nullptr;
        }
    }

    namespace SpecRoles
    {
        SpecRole GetRole(uint8 classId, uint8 specIndex)
        {
            SpecInfo const* s = Find(classId, specIndex);
            return s ? s->role : SpecRole::Dps;
        }

        std::string GetSpecName(uint8 classId, uint8 specIndex)
        {
            SpecInfo const* s = Find(classId, specIndex);
            return s ? std::string(s->name) : std::string();
        }

        uint8 GetBotSpecIndex(Player* bot)
        {
            if (!bot)
                return 0;
            if (uint32 specId = bot->GetPrimarySpecialization())
                if (ChrSpecializationEntry const* spec = sChrSpecializationStore.LookupEntry(specId))
                    return static_cast<uint8>(spec->OrderIndex);
            return 0;
        }

        SpecRole GetBotRole(Player* bot)
        {
            if (!bot)
                return SpecRole::Dps;
            return GetRole(bot->getClass(), GetBotSpecIndex(bot));
        }

        bool IsMeleeClass(uint8 classId)
        {
            switch (classId)
            {
                case CLASS_WARRIOR:
                case CLASS_PALADIN:
                case CLASS_ROGUE:
                case CLASS_DEATH_KNIGHT:
                case CLASS_MONK:
                case CLASS_DEMON_HUNTER:
                    return true;
                default:
                    return false;   // hunter/mage/warlock/priest/shaman/druid: ranged-leaning
            }
        }
    }
}
