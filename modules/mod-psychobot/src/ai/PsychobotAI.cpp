/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "PsychobotAI.h"
#include "PsychobotAiFactory.h"
#include "PsychobotTalentMgr.h"
#include "PsychobotGearMgr.h"
#include "../PsychobotPopulationMgr.h"
#include "../PsychobotGroupMgr.h"
#include "../travel/PsychobotTravelMgr.h"
#include "../pvp/PsychobotPvpMgr.h"
#include "../dungeon/PsychobotDungeonMgr.h"
#include "../world/PsychobotDbStore.h"
#include "../PsychobotAIFwd.h"
#include "../engine/AiObjectContext.h"
#include "../engine/Engine.h"
#include "../engine/ServerFacade.h"
#include "Player.h"
#include "Unit.h"
#include "Config.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "DB2Stores.h"
#include "DBCEnums.h"
#include "Common.h"
#include <algorithm>
#include <cctype>
#include <cmath>

namespace psychobot
{
    // Run the decision engine ~ every 250ms (4x/sec).
    static uint32 const PSYCHOBOT_AI_TICK_MS = 250;

    PsychobotAI::PsychobotAI(Player* bot, ObjectGuid masterGuid)
        : _bot(bot), _masterGuid(masterGuid), _state(BotState::NonCombat),
          _updateDelay(0), _specApplied(false), _currentEngine(nullptr)
    {
        // Build the bot's context (generic + class vocabulary/behaviour).
        _context.reset(AiFactory::CreateContext(this));

        // S27/persistence: load this bot's saved (master-toggled) strategies
        // from the characters DB (table psychobot_strategies) so InitCombatEngine
        // below can re-apply them via DbStore::GetStrategies.
        if (_bot)
            sPsychobotDbStore->Load(_bot->GetGUID());

        // One engine per state, sharing the single context.
        _nonCombatEngine = std::make_unique<Engine>(this, _context.get(), BotState::NonCombat);
        _combatEngine    = std::make_unique<Engine>(this, _context.get(), BotState::Combat);
        _deadEngine      = std::make_unique<Engine>(this, _context.get(), BotState::Dead);

        // Install starting strategies.
        AiFactory::InitNonCombatEngine(this, _nonCombatEngine.get());
        AiFactory::InitCombatEngine(this, _combatEngine.get());
        AiFactory::InitDeadEngine(this, _deadEngine.get());

        _currentEngine = _nonCombatEngine.get();
    }

    PsychobotAI::~PsychobotAI() = default;

    Player* PsychobotAI::GetMaster() const
    {
        if (_masterGuid.IsEmpty())
            return nullptr;
        return ObjectAccessor::FindConnectedPlayer(_masterGuid);
    }

    void PsychobotAI::EnsureSpecAndTalents()
    {
        if (_specApplied || !_bot)
            return;
        TalentMgr::SetupSpec(_bot, /*specIndex*/ -1, /*preferColumn*/ 0);
        _specApplied = true;

        // The combat engine's strategy depends on the (now-applied) spec role,
        // so rebuild its starting strategies once after spec setup.
        if (_combatEngine)
        {
            _combatEngine->ClearStrategies();
            AiFactory::InitCombatEngine(this, _combatEngine.get());

            // S27: re-apply the master's saved extra combat strategies.
            if (_bot)
                for (std::string const& s : sPsychobotDbStore->GetStrategies(_bot->GetGUID()))
                    _combatEngine->AddStrategy(s);
        }
    }

    // ----------------------------------------------------------------------
    // S27 command surface
    // ----------------------------------------------------------------------
    bool PsychobotAI::ToggleCombatStrategy(std::string const& name)
    {
        if (!_combatEngine || !_bot)
            return false;
        bool nowOn = sPsychobotDbStore->ToggleStrategy(_bot->GetGUID(), name);
        if (nowOn)
            _combatEngine->AddStrategy(name);
        else
            _combatEngine->RemoveStrategy(name);
        // S27/persistence: write the updated set to the characters DB
        // (table psychobot_strategies) so it survives a relog.
        sPsychobotDbStore->Save(_bot->GetGUID());
        return nowOn;
    }

    bool PsychobotAI::OrderAttackMasterTarget()
    {
        Player* master = GetMaster();
        if (!_bot || !master)
            return false;
        Unit* sel = master->GetSelectedUnit();
        if (!sel || !_bot->IsValidAttackTarget(sel))
            return false;
        _staying = false;   // attacking implies we may move
        return AttackTarget(sel);
    }

    bool PsychobotAI::OrderStay()
    {
        _staying = true;
        return StopMoving();
    }

    bool PsychobotAI::OrderFollow()
    {
        _staying = false;
        return FollowMaster();
    }

    bool PsychobotAI::OrderCast(std::string const& spellName)
    {
        Player* master = GetMaster();
        if (!_bot || !master)
            return false;
        uint32 id = GetSpellIdByName(spellName);
        if (!id)
            return false;
        Unit* sel = master->GetSelectedUnit();
        Unit* target = (sel && _bot->IsValidAttackTarget(sel)) ? sel : static_cast<Unit*>(_bot);
        return CastSpell(id, target);
    }

    std::string PsychobotAI::ListCombatStrategies() const
    {
        return _combatEngine ? _combatEngine->ListStrategies() : std::string();
    }

    void PsychobotAI::UpdateState()
    {
        BotState desired;
        if (!_bot->IsAlive())
            desired = BotState::Dead;
        else if (_bot->IsInCombat())
            desired = BotState::Combat;
        else
            desired = BotState::NonCombat;

        if (desired != _state)
        {
            _state = desired;
            switch (_state)
            {
                case BotState::Combat:    _currentEngine = _combatEngine.get();    break;
                case BotState::Dead:      _currentEngine = _deadEngine.get();      break;
                case BotState::NonCombat: _currentEngine = _nonCombatEngine.get(); break;
            }
        }
    }

    void PsychobotAI::UpdateAI(uint32 diff)
    {
        if (!_bot || !_bot->IsInWorld())
            return;

        _updateDelay += diff;
        if (_updateDelay < PSYCHOBOT_AI_TICK_MS)
            return;
        _updateDelay = 0;

        // Scaling gate (SmartScale / BotActiveAlone / DisabledWithoutRealPlayer).
        if (!sPsychobotPopulation->ShouldBotBeActive(_bot))
            return;

        EnsureSpecAndTalents();

        // Keep gear roughly level-appropriate (cheap; persistence-gated).
        GearMgr::GearUp(_bot);

        // S20 group coordination (cheap, every tick):
        //  - bots auto-accept ready checks (always ready)
        //  - tank/leader bots mark their target with Skull so the group focuses
        BotGroupMgr::AnswerReadyCheck(_bot);
        if (_bot->IsInCombat())
            BotGroupMgr::MarkSkullTarget(_bot);

        // S25: react to the current boss encounter's mechanics (move out of
        // fire, etc.). No-op outside instances / when no script is registered.
        if (_bot->IsInCombat() && DungeonMgr::InDungeonOrRaid(_bot))
            DungeonMgr::RunEncounterScript(_bot);

        UpdateState();

        // Dead bots stay idle until release/rez handling (later step).
        if (_state == BotState::Dead)
            return;

        if (_currentEngine)
            _currentEngine->DoNextAction();
    }

    // ----------------------------------------------------------------------
    // Cast / target seam
    // ----------------------------------------------------------------------
    Unit* PsychobotAI::GetCurrentTarget() const
    {
        if (!_bot)
            return nullptr;

        if (Unit* victim = _bot->GetVictim())
            return victim;

        if (Unit* assist = BotGroupMgr::GetGroupAssistTarget(_bot))
            return assist;

        // S24: in an active battleground/arena, hunt the nearest enemy player.
        if (PvpMgr::InActiveBattleground(_bot))
            if (Unit* foe = PvpMgr::GetPvpTarget(_bot, 60.0f))
                return foe;

        if (Player* master = GetMaster())
            if (Unit* sel = master->GetSelectedUnit())
                return sel;

        return nullptr;
    }

    uint32 PsychobotAI::GetSpellIdByName(std::string const& name) const
    {
        if (name.empty() || !_bot)
            return 0;

        std::string lower = name;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        for (SpellNameEntry const* entry : sSpellNameStore)
        {
            if (!entry)
                continue;

            char const* spellName = nullptr;
            if (entry->Name)
                spellName = (*entry->Name)[LOCALE_enUS];
            if (!spellName || !*spellName)
                continue;

            std::string sn = spellName;
            std::transform(sn.begin(), sn.end(), sn.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            if (sn == lower && _bot->HasSpell(entry->ID))
                return entry->ID;
        }
        return 0;
    }

    bool PsychobotAI::CastSpell(uint32 spellId, Unit* target)
    {
        if (!_bot || !spellId || !target)
            return false;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
            return false;

        if (!_bot->HasSpell(spellId))
            return false;

        if (_bot->IsNonMeleeSpellCast(false))
            return false;

        _bot->CastSpell(target, spellId, false);
        return true;
    }

    // ----------------------------------------------------------------------
    // Movement seam
    // ----------------------------------------------------------------------
    bool PsychobotAI::ReachTarget(Unit* target, float distance)
    {
        if (!_bot || !target)
            return false;
        if (target->GetMap() != _bot->GetMap())
            return false;
        _bot->GetMotionMaster()->MoveChase(target, distance);
        return true;
    }

    bool PsychobotAI::FollowMaster()
    {
        if (_staying)
            return false;   // S27: "stay" order holds position
        Player* master = GetMaster();
        if (!_bot || !master || !master->IsInWorld())
            return false;

        // Configurable trail distance (Psychobot.FollowDistance, default 2.0y).
        float const followDist = sConfigMgr->GetFloatDefault("Psychobot.FollowDistance", 2.0f);

        // S23: travel catch-up first. If the master is on another map or beyond
        // the leash, this teleports us in and returns true (we're done this tick).
        if (TravelMgr::FollowMasterTravel(_bot, master, followDist))
            return true;

        // Same map + within leash: normal follow with the S20 formation angle.
        float angle = BotGroupMgr::GetFollowFormationAngle(_bot);
        _bot->GetMotionMaster()->MoveFollow(master, followDist, angle);
        return true;
    }

    bool PsychobotAI::StopMoving()
    {
        if (!_bot)
            return false;
        _bot->GetMotionMaster()->Clear();
        return true;
    }

    bool PsychobotAI::AttackTarget(Unit* target)
    {
        if (!_bot || !target)
            return false;
        if (!_bot->IsValidAttackTarget(target))
            return false;

        // Set selection + begin/maintain auto-attack, and chase into range.
        _bot->SetSelection(target->GetGUID());
        _bot->Attack(target, true);
        _bot->GetMotionMaster()->MoveChase(target);
        return true;
    }

    // ----------------------------------------------------------------------
    // Engine bridge (declared in PsychobotAIFwd.h)
    // ----------------------------------------------------------------------
    namespace PsychobotAIBridge
    {
        Player* GetBot(PsychobotAI* ai)    { return ai ? ai->GetBot() : nullptr; }
        Player* GetMaster(PsychobotAI* ai) { return ai ? ai->GetMaster() : nullptr; }
        Unit*   GetCurrentTarget(PsychobotAI* ai) { return ai ? ai->GetCurrentTarget() : nullptr; }
        AiObjectContext* GetContext(PsychobotAI* ai) { return ai ? ai->GetContext() : nullptr; }

        uint32 GetSpellId(PsychobotAI* ai, std::string const& name) { return ai ? ai->GetSpellIdByName(name) : 0; }
        bool   CastSpell(PsychobotAI* ai, uint32 spellId, Unit* target) { return ai && ai->CastSpell(spellId, target); }
        bool   ReachTarget(PsychobotAI* ai, Unit* target, float distance) { return ai && ai->ReachTarget(target, distance); }
        bool   FollowMaster(PsychobotAI* ai) { return ai && ai->FollowMaster(); }
        bool   StopMoving(PsychobotAI* ai) { return ai && ai->StopMoving(); }
        bool   AttackTarget(PsychobotAI* ai, Unit* target) { return ai && ai->AttackTarget(target); }

        // --- pet control seam (S19) -----------------------------------------
        bool PetAttack(PsychobotAI* ai, Unit* target)
        {
            Player* bot = ai ? ai->GetBot() : nullptr;
            return bot && ServerFacade::PetAttack(bot, target);
        }
        bool PetFollow(PsychobotAI* ai)
        {
            Player* bot = ai ? ai->GetBot() : nullptr;
            return bot && ServerFacade::PetFollow(bot);
        }
        bool PetCastSpell(PsychobotAI* ai, std::string const& name, Unit* target)
        {
            if (!ai)
                return false;
            Player* bot = ai->GetBot();
            if (!bot)
                return false;
            uint32 spellId = ai->GetSpellIdByName(name);
            return spellId && ServerFacade::PetCastSpell(bot, spellId, target);
        }
    }
}
