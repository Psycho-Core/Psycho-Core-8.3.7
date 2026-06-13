/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotAI - per-bot brain. Wraps a real TrinityCore Player and drives it
 *  each tick through the REAL relevance-priority engine (the fat port). It owns
 *  one AiObjectContext (the value/trigger/action/strategy registry) and three
 *  per-state Engines (non-combat / combat / dead); UpdateState() swaps the
 *  active engine and DoNextAction() runs one decision tick.
 *
 *  It also hosts the ServerFacade-style cast/move seam the engine calls back
 *  into via PsychobotAIBridge, so actions never touch the core API directly.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_AI_H
#define PSYCHOBOT_AI_H

#include "Define.h"
#include "ObjectGuid.h"
#include "../engine/Strategy.h"   // BotState
#include <memory>

class Player;
class Unit;

namespace psychobot
{
    class AiObjectContext;
    class Engine;

    class PsychobotAI
    {
    public:
        PsychobotAI(Player* bot, ObjectGuid masterGuid);
        ~PsychobotAI();

        // Driven each world tick by PsychobotMgr.
        void UpdateAI(uint32 diff);

        Player*    GetBot() const { return _bot; }
        Player*    GetMaster() const;             // resolved live (may be null)
        ObjectGuid GetMasterGuid() const { return _masterGuid; }
        AiObjectContext* GetContext() const { return _context.get(); }
        BotState   GetState() const { return _state; }

        // --- the cast/move seam the engine calls back into ----------------
        uint32 GetSpellIdByName(std::string const& name) const;
        bool   CastSpell(uint32 spellId, Unit* target);
        Unit*  GetCurrentTarget() const;          // victim / assist / selection
        bool   ReachTarget(Unit* target, float distance);
        bool   FollowMaster();
        bool   StopMoving();
        bool   AttackTarget(Unit* target);

        // Apply BfA spec + talents to this bot (idempotent).
        void EnsureSpecAndTalents();

        // --- S27 command surface (chat-driven control) --------------------
        // Toggle an extra combat strategy on/off (persisted via DbStore).
        // Returns the new state (true = now active).
        bool ToggleCombatStrategy(std::string const& name);
        // One-shot orders from the master:
        bool OrderAttackMasterTarget();   // attack what the master has selected
        bool OrderStay();                 // stop + hold position (no follow)
        bool OrderFollow();               // resume following the master
        bool OrderCast(std::string const& spellName);  // cast on master's target
        // Active strategies of the combat engine, comma-joined (for help/status).
        std::string ListCombatStrategies() const;

    private:
        void UpdateState();                       // combat/non-combat engine swap

        Player*    _bot;
        ObjectGuid _masterGuid;
        BotState   _state;
        uint32     _updateDelay;                  // ms accumulator (throttle)
        bool       _specApplied;
        bool       _staying = false;              // S27: "stay" order suppresses follow

        std::unique_ptr<AiObjectContext> _context;
        std::unique_ptr<Engine> _nonCombatEngine;
        std::unique_ptr<Engine> _combatEngine;
        std::unique_ptr<Engine> _deadEngine;
        Engine*    _currentEngine;
    };
}

#endif // PSYCHOBOT_AI_H
