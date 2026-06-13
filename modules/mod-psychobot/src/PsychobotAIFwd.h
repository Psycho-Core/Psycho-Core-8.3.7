/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Bridge so the engine layer can reach the bot/master Player* of a PsychobotAI
 *  WITHOUT including the full PsychobotAI header (avoids a circular dependency:
 *  PsychobotAI owns the engine; the engine only needs these two accessors).
 *  The bridge is implemented in PsychobotAI.cpp.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_AI_FWD_H
#define PSYCHOBOT_AI_FWD_H

class Player;
class Unit;

namespace psychobot
{
    class PsychobotAI;
    class AiObjectContext;

    namespace PsychobotAIBridge
    {
        Player* GetBot(PsychobotAI* ai);
        Player* GetMaster(PsychobotAI* ai);
        // The unit the bot should currently be acting on (victim / assist /
        // master's selection). Resolved live; may be nullptr.
        Unit*   GetCurrentTarget(PsychobotAI* ai);
        // The bot's AiObjectContext (the value/trigger/action/strategy registry).
        AiObjectContext* GetContext(PsychobotAI* ai);

        // --- action seam (route action Execute()s back through the AI) -------
        // Resolve a spell name (case-insensitive) to a spell the bot knows.
        uint32 GetSpellId(PsychobotAI* ai, std::string const& name);
        // Cast spellId on target (false if unknown / on CD / already casting).
        bool   CastSpell(PsychobotAI* ai, uint32 spellId, Unit* target);
        // Movement.
        bool   ReachTarget(PsychobotAI* ai, Unit* target, float distance);
        bool   FollowMaster(PsychobotAI* ai);
        bool   StopMoving(PsychobotAI* ai);
        // Start auto-attacking target (melee swing).
        bool   AttackTarget(PsychobotAI* ai, Unit* target);

        // --- pet control seam (S19) -----------------------------------------
        bool   PetAttack(PsychobotAI* ai, Unit* target);
        bool   PetFollow(PsychobotAI* ai);
        bool   PetCastSpell(PsychobotAI* ai, std::string const& name, Unit* target);
    }
}

#endif // PSYCHOBOT_AI_FWD_H
