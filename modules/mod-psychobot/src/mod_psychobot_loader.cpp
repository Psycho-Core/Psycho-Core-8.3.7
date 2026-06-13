/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Module loader. The generated ModulesLoader calls Add<folder>Scripts() with
 *  hyphens converted to underscores, so "mod-psychobot" expects
 *  "Addmod_psychobotScripts".
 * ===========================================================================
 */

// Forward declarations of this module's per-file registrators.
void AddSC_mod_psychobot();

// Entry point invoked by the module loader (folder name -> underscores).
void Addmod_psychobotScripts()
{
    AddSC_mod_psychobot();
}
