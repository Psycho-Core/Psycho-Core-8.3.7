/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "ServerFacade.h"
#include "Player.h"
#include "Unit.h"
#include "Pet.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "SpellHistory.h"
#include "DBCEnums.h"
#include "UnitDefines.h"   // ReactStates (REACT_AGGRESSIVE / REACT_DEFENSIVE)
#include "SpellAuras.h"    // Aura / AuraApplication (S21 dispel)
#include "SharedDefines.h" // DispelType, POWER_MANA

namespace psychobot
{
    namespace ServerFacade
    {
        bool IsAlive(Unit* unit) { return unit && unit->IsAlive(); }

        bool IsInCombat(Unit* unit) { return unit && unit->IsInCombat(); }

        bool IsFriendly(Unit* a, Unit* b) { return a && b && a->IsFriendlyTo(b); }

        bool IsHostile(Unit* a, Unit* b) { return a && b && a->IsHostileTo(b); }

        bool IsValidAttackTarget(Unit* attacker, Unit* target)
        {
            return attacker && target && attacker->IsValidAttackTarget(target);
        }

        Unit* GetVictim(Unit* unit) { return unit ? unit->GetVictim() : nullptr; }

        float GetDistance2d(Unit* from, Unit* to)
        {
            if (!from || !to)
                return 99999.0f;
            return from->GetDistance(to);
        }

        bool IsInRange(Unit* from, Unit* to, float range)
        {
            return from && to && from->IsWithinDist(to, range);
        }

        bool IsInMeleeRange(Unit* from, Unit* to)
        {
            return from && to && from->IsWithinMeleeRange(to);
        }

        bool IsInFront(Unit* from, Unit* to)
        {
            return from && to && from->isInFront(to);
        }

        float GetHealthPct(Unit* unit) { return unit ? unit->GetHealthPct() : 0.0f; }

        float GetPowerPct(Unit* unit, uint32 powerType)
        {
            return unit ? unit->GetPowerPct(Powers(powerType)) : 0.0f;
        }

        uint32 GetPower(Unit* unit, uint32 powerType)
        {
            if (!unit)
                return 0;
            int32 p = unit->GetPower(Powers(powerType));
            return p > 0 ? uint32(p) : 0u;
        }

        bool HasAura(Unit* unit, uint32 spellId)
        {
            return unit && spellId && unit->HasAura(spellId);
        }

        bool IsCasting(Unit* unit)
        {
            return unit && unit->IsNonMeleeSpellCast(false);
        }

        bool KnowsSpell(Player* bot, uint32 spellId)
        {
            return bot && spellId && bot->HasSpell(spellId);
        }

        bool IsSpellReady(Player* bot, uint32 spellId)
        {
            if (!bot || !spellId || !bot->HasSpell(spellId))
                return false;

            SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId, DIFFICULTY_NONE);
            if (!info)
                return false;

            SpellHistory* history = bot->GetSpellHistory();
            if (history && history->HasCooldown(info))
                return false;

            return true;
        }

        bool IsMoving(Unit* unit) { return unit && unit->isMoving(); }

        Unit* GetPet(Player* bot)
        {
            if (!bot)
                return nullptr;
            Pet* pet = bot->GetPet();
            if (!pet || !pet->IsAlive())
                return nullptr;
            return pet;   // Pet IS-A Unit
        }

        bool HasLivingPet(Player* bot)
        {
            return GetPet(bot) != nullptr;
        }

        float GetPetHealthPct(Player* bot)
        {
            Unit* pet = GetPet(bot);
            return pet ? pet->GetHealthPct() : 0.0f;
        }

        // --- pet control ----------------------------------------------------
        bool PetAttack(Player* bot, Unit* target)
        {
            if (!bot || !target)
                return false;
            Pet* pet = bot->GetPet();
            if (!pet || !pet->IsAlive())
                return false;
            if (!pet->IsValidAttackTarget(target))
                return false;

            pet->SetReactState(REACT_AGGRESSIVE);
            // Already on it? don't restart the swing.
            if (pet->GetVictim() == target)
                return true;
            pet->AttackStop();
            pet->Attack(target, /*meleeAttack*/ true);
            return true;
        }

        bool PetFollow(Player* bot)
        {
            if (!bot)
                return false;
            Pet* pet = bot->GetPet();
            if (!pet || !pet->IsAlive())
                return false;
            pet->SetReactState(REACT_DEFENSIVE);
            pet->AttackStop();
            return true;
        }

        bool PetAttackingTarget(Player* bot, Unit* target)
        {
            if (!bot || !target)
                return false;
            Pet* pet = bot->GetPet();
            return pet && pet->IsAlive() && pet->GetVictim() == target;
        }

        bool PetSpellReady(Player* bot, uint32 spellId)
        {
            if (!bot || !spellId)
                return false;
            Pet* pet = bot->GetPet();
            if (!pet || !pet->IsAlive())
                return false;
            SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId, DIFFICULTY_NONE);
            if (!info)
                return false;
            if (SpellHistory* h = pet->GetSpellHistory())
                if (h->HasCooldown(info))
                    return false;
            return true;
        }

        bool PetCastSpell(Player* bot, uint32 spellId, Unit* target)
        {
            if (!bot || !spellId || !target)
                return false;
            Pet* pet = bot->GetPet();
            if (!pet || !pet->IsAlive())
                return false;
            if (!PetSpellReady(bot, spellId))
                return false;
            pet->CastSpell(target, spellId, false);
            return true;
        }

        // --- healing depth (S21) --------------------------------------------
        bool HasDispellableAura(Unit* unit, uint32 dispelMask)
        {
            if (!unit || !dispelMask)
                return false;

            for (auto const& pair : unit->GetAppliedAuras())
            {
                AuraApplication const* app = pair.second;
                if (!app || app->IsPositive())
                    continue;   // only remove harmful auras
                Aura const* aura = app->GetBase();
                if (!aura)
                    continue;
                SpellInfo const* info = aura->GetSpellInfo();
                if (!info)
                    continue;
                // info->Dispel is the DispelType; test it against the mask.
                if (dispelMask & (1u << info->Dispel))
                    return true;
            }
            return false;
        }

        float GetManaPct(Unit* unit)
        {
            if (!unit)
                return 0.0f;
            if (unit->GetPowerType() != POWER_MANA)
                return 0.0f;
            return unit->GetPowerPct(POWER_MANA);
        }
    }
}
