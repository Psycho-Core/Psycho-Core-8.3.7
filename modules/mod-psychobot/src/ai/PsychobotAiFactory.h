/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  PsychobotAiFactory - assembles a bot's brain. It (1) builds the base
 *  AiObjectContext by registering the generic values/triggers/actions/strategies
 *  (S4/S5/S6), then layers each class' own context on top (S7+), and (2) wires
 *  the per-state engines with the right starting strategies for the bot's
 *  class + spec role (melee/ranged/tank/heal + follow).
 *
 *  This is the single integration seam: PsychobotAI owns one context and three
 *  engines and calls these builders once.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_AI_FACTORY_H
#define PSYCHOBOT_AI_FACTORY_H

class Player;

namespace psychobot
{
    class PsychobotAI;
    class AiObjectContext;
    class Engine;

    namespace AiFactory
    {
        // Build + populate the base (and class) context for this bot.
        AiObjectContext* CreateContext(PsychobotAI* ai);

        // Install the starting strategies into each per-state engine.
        void InitNonCombatEngine(PsychobotAI* ai, Engine* engine);
        void InitCombatEngine(PsychobotAI* ai, Engine* engine);
        void InitDeadEngine(PsychobotAI* ai, Engine* engine);
    }
}

#endif // PSYCHOBOT_AI_FACTORY_H
