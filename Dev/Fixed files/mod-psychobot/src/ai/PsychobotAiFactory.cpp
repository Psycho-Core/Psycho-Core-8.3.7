/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotAiFactory.h"
#include "PsychobotSpecRoles.h"
#include "../engine/AiObjectContext.h"
#include "../engine/Engine.h"
#include "../engine/Strategy.h"
#include "../values/CoreValues.h"
#include "../triggers/CoreTriggers.h"
#include "../actions/GenericSpellActions.h"
#include "../actions/MovementActions.h"
#include "../actions/WorldActions.h"
#include "../strategies/GenericStrategies.h"
#include "../classes/deathknight/DKAiObjectContext.h"
#include "../classes/warrior/WarriorAiObjectContext.h"
#include "../classes/paladin/PaladinAiObjectContext.h"
#include "../classes/hunter/HunterAiObjectContext.h"
#include "../classes/rogue/RogueAiObjectContext.h"
#include "../classes/priest/PriestAiObjectContext.h"
#include "../classes/shaman/ShamanAiObjectContext.h"
#include "../classes/mage/MageAiObjectContext.h"
#include "../classes/warlock/WarlockAiObjectContext.h"
#include "../classes/druid/DruidAiObjectContext.h"
#include "../classes/monk/MonkAiObjectContext.h"
#include "../classes/demonhunter/DemonHunterAiObjectContext.h"
#include "../pets/PetStrategy.h"
#include "../PsychobotAIFwd.h"
#include "Player.h"
#include "SharedDefines.h"   // CLASS_*

namespace psychobot
{
    namespace AiFactory
    {
        // Does this class+spec rely on a controllable pet (S19)?
        //   Hunter BeastMastery(0) + Survival(2); all Warlock; DK Unholy(2);
        //   Mage Frost(2). (Hunter Marksmanship can pet too but doesn't need it.)
        static bool UsesPet(uint8 classId, uint8 specIndex)
        {
            switch (classId)
            {
                case CLASS_HUNTER:       return specIndex == 0 || specIndex == 2;
                case CLASS_WARLOCK:      return true;
                case CLASS_DEATH_KNIGHT: return specIndex == 2;   // Unholy ghoul
                case CLASS_MAGE:         return specIndex == 2;   // Frost water elemental
                default:                 return false;
            }
        }

        AiObjectContext* CreateContext(PsychobotAI* ai)
        {
            AiObjectContext* context = new AiObjectContext(ai);

            // Base context: generic vocabulary + behaviour (S4/S5/S6).
            RegisterCoreValues(context);
            RegisterCoreTriggers(context);
            RegisterGenericSpellActions(context);
            RegisterMovementActions(context);
            RegisterWorldActions(context);      // S22 rest/repair actions
            RegisterGenericStrategies(context);

            // Per-class contexts (S7+) layered on top of the base for this bot.
            Player* bot = PsychobotAIBridge::GetBot(ai);
            uint8 classId = bot ? bot->getClass() : 0;
            switch (classId)
            {
                case CLASS_DEATH_KNIGHT: DeathKnight::RegisterContext(context); break;
                case CLASS_WARRIOR:      Warrior::RegisterContext(context);    break;
                case CLASS_PALADIN:      Paladin::RegisterContext(context);    break;
                case CLASS_HUNTER:       Hunter::RegisterContext(context);     break;
                case CLASS_ROGUE:        Rogue::RegisterContext(context);      break;
                case CLASS_PRIEST:       Priest::RegisterContext(context);     break;
                case CLASS_SHAMAN:       Shaman::RegisterContext(context);     break;
                case CLASS_MAGE:         Mage::RegisterContext(context);       break;
                case CLASS_WARLOCK:      Warlock::RegisterContext(context);    break;
                case CLASS_DRUID:        Druid::RegisterContext(context);      break;
                case CLASS_MONK:         Monk::RegisterContext(context);       break;
                case CLASS_DEMON_HUNTER: DemonHunter::RegisterContext(context); break;
                default: break;   // all 12 classes registered
            }

            // S19: pet system (actions/trigger/strategy) for pet-using classes.
            if (UsesPet(classId, SpecRoles::GetBotSpecIndex(bot)))
                RegisterPetSystem(context);

            return context;
        }

        void InitNonCombatEngine(PsychobotAI* ai, Engine* engine)
        {
            if (!engine)
                return;

            engine->AddStrategy("follow");
            engine->AddStrategy("rest");   // S22: eat/drink/repair out of combat

            // Class-specific non-combat upkeep.
            Player* bot = PsychobotAIBridge::GetBot(ai);
            uint8 classId = bot ? bot->getClass() : 0;
            if (classId == CLASS_DEATH_KNIGHT)
                engine->AddStrategy("dk nc");
            else if (classId == CLASS_WARRIOR)
                engine->AddStrategy("warrior nc");
            else if (classId == CLASS_PALADIN)
                engine->AddStrategy("paladin nc");
            else if (classId == CLASS_HUNTER)
                engine->AddStrategy("hunter nc");
            else if (classId == CLASS_ROGUE)
                engine->AddStrategy("rogue nc");
            else if (classId == CLASS_PRIEST)
                engine->AddStrategy("priest nc");
            else if (classId == CLASS_SHAMAN)
                engine->AddStrategy("shaman nc");
            else if (classId == CLASS_MAGE)
                engine->AddStrategy("mage nc");
            else if (classId == CLASS_WARLOCK)
                engine->AddStrategy("warlock nc");
            else if (classId == CLASS_DRUID)
                engine->AddStrategy("druid nc");
            else if (classId == CLASS_MONK)
                engine->AddStrategy("monk nc");
            else if (classId == CLASS_DEMON_HUNTER)
                engine->AddStrategy("demon hunter nc");
        }

        void InitCombatEngine(PsychobotAI* ai, Engine* engine)
        {
            if (!engine)
                return;

            Player* bot = PsychobotAIBridge::GetBot(ai);
            uint8 classId = bot ? bot->getClass() : 0;
            SpecRole role = SpecRoles::GetBotRole(bot);
            uint8 specIndex = SpecRoles::GetBotSpecIndex(bot);

            // S19: pet coordination layered on top of the spec rotation for pet
            // classes. AddStrategy is a no-op if "pet" wasn't registered, so this
            // is safe for non-pet classes. (Added before the early-returning
            // class branches below so every pet spec gets it.)
            if (UsesPet(classId, specIndex))
                engine->AddStrategy("pet");

            // --- class-specific combat strategies (S7+) ----------------------
            if (classId == CLASS_DEATH_KNIGHT)
            {
                // Spec rotation (blood/frost/unholy) on top of the shared DK base.
                engine->AddStrategy("dk");
                engine->AddStrategy(DeathKnight::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_WARRIOR)
            {
                // Spec rotation (arms/fury/protection) on the shared warrior base.
                engine->AddStrategy("warrior");
                engine->AddStrategy(Warrior::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_PALADIN)
            {
                // Spec rotation (holy/protection/retribution) on the paladin base.
                engine->AddStrategy("paladin");
                engine->AddStrategy(Paladin::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_HUNTER)
            {
                // Spec rotation (beast mastery/marksmanship/survival) + hunter base.
                engine->AddStrategy("hunter");
                engine->AddStrategy(Hunter::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_ROGUE)
            {
                // Spec rotation (assassination/outlaw/subtlety) + rogue base.
                engine->AddStrategy("rogue");
                engine->AddStrategy(Rogue::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_PRIEST)
            {
                // Spec rotation (discipline/holy/shadow) + priest base.
                engine->AddStrategy("priest");
                engine->AddStrategy(Priest::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_SHAMAN)
            {
                // Spec rotation (elemental/enhancement/restoration) + shaman base.
                engine->AddStrategy("shaman");
                engine->AddStrategy(Shaman::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_MAGE)
            {
                // Spec rotation (arcane/fire/frost) + mage base.
                engine->AddStrategy("mage");
                engine->AddStrategy(Mage::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_WARLOCK)
            {
                // Spec rotation (affliction/demonology/destruction) + warlock base.
                engine->AddStrategy("warlock");
                engine->AddStrategy(Warlock::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_DRUID)
            {
                // Spec rotation (balance/feral/guardian/restoration) + druid base.
                engine->AddStrategy("druid");
                engine->AddStrategy(Druid::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_MONK)
            {
                // Spec rotation (brewmaster/mistweaver/windwalker) + monk base.
                engine->AddStrategy("monk");
                engine->AddStrategy(Monk::CombatStrategyForSpec(specIndex));
                return;
            }
            if (classId == CLASS_DEMON_HUNTER)
            {
                // Spec rotation (havoc/vengeance) + demon hunter base.
                engine->AddStrategy("demon hunter");
                engine->AddStrategy(DemonHunter::CombatStrategyForSpec(specIndex));
                return;
            }

            // --- generic fallback for not-yet-ported classes -----------------
            switch (role)
            {
                case SpecRole::Tank:
                    engine->AddStrategy("tank");
                    break;
                case SpecRole::Healer:
                    engine->AddStrategy("heal");
                    // Healers still auto-attack when nobody needs healing.
                    engine->AddStrategy(SpecRoles::IsMeleeClass(classId) ? "melee" : "ranged");
                    break;
                case SpecRole::Dps:
                default:
                    engine->AddStrategy(SpecRoles::IsMeleeClass(classId) ? "melee" : "ranged");
                    break;
            }
        }

        void InitDeadEngine(PsychobotAI* /*ai*/, Engine* /*engine*/)
        {
            // Release/resurrect handled in a later step; dead engine is idle now.
        }
    }
}
