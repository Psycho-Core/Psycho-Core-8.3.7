/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotDungeonMgr.h"
#include "../PsychobotAIFwd.h"      // (not strictly needed; kept for symmetry)
#include "Player.h"
#include "Unit.h"
#include "Creature.h"
#include "Map.h"
#include "Spell.h"
#include "SpellInfo.h"
#include <map>

namespace psychobot
{
    namespace DungeonMgr
    {
        bool InDungeonOrRaid(Player* bot)
        {
            if (!bot || !bot->IsInWorld())
                return false;
            Map* map = bot->GetMap();
            return map && (map->IsDungeon() || map->IsRaid());
        }

        // Helper: the boss the bot is currently fighting, or nullptr.
        static Creature* GetEngagedBoss(Player* bot)
        {
            if (!bot)
                return nullptr;
            Unit* victim = bot->GetVictim();
            if (!victim)
                return nullptr;
            Creature* creature = victim->ToCreature();
            if (!creature)
                return nullptr;
            if (!creature->IsDungeonBoss() && !creature->isWorldBoss())
                return nullptr;
            return creature;
        }

        bool FightingBoss(Player* bot)
        {
            return GetEngagedBoss(bot) != nullptr;
        }

        bool BossCastingInterruptible(Player* bot)
        {
            Creature* boss = GetEngagedBoss(bot);
            if (!boss)
                return false;

            // Check the boss's active generic / channeled cast.
            for (uint32 t = CURRENT_GENERIC_SPELL; t <= CURRENT_CHANNELED_SPELL; ++t)
            {
                Spell* spell = boss->GetCurrentSpell(t);
                if (!spell)
                    continue;
                SpellInfo const* info = spell->GetSpellInfo();
                if (!info)
                    continue;
                if (info->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT)
                    return true;
            }
            return false;
        }

        // --- per-encounter script framework ---------------------------------
        namespace
        {
            std::map<uint32, EncounterScript>& Scripts()
            {
                static std::map<uint32, EncounterScript> scripts;
                return scripts;
            }
        }

        void RegisterEncounter(uint32 creatureEntry, EncounterScript script)
        {
            if (script)
                Scripts()[creatureEntry] = std::move(script);
        }

        bool RunEncounterScript(Player* bot)
        {
            Creature* boss = GetEngagedBoss(bot);
            if (!boss)
                return false;
            auto it = Scripts().find(boss->GetEntry());
            if (it == Scripts().end() || !it->second)
                return false;
            return it->second(bot, boss);
        }

        void InitEncounters()
        {
            // SEED examples. Concrete reactions (move out of a specific ground
            // effect, soak, spread) need per-encounter positions/spell ids and
            // are documented hooks; these placeholders show the registration
            // pattern and are safe no-ops until filled in on a running server.
            //
            // Example (commented - fill entry + reaction on-server):
            // RegisterEncounter(/*bossEntry*/ 0, [](Player* bot, Unit* boss) -> bool
            // {
            //     // e.g. if (bot is in boss's fire) MoveTo(safe spot); return true;
            //     return false;
            // });
        }
    }
}
