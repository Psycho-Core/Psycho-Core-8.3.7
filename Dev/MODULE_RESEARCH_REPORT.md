# Module System and mod-psychobot Research Report

## Documentation reviewed
- Psycho `README.md`: modernized 8.3.7 core, toolchain targets, modules, TDB notes.
- `modules/README.md`: module discovery/layout, `MODULES=static|dynamic|none`, config copy, SQL registration.
- `modules/mod-psychobot/README.md`: commands, architecture, SQL, requirements, honest gaps.
- `Dev/BFA_Content_Systems_Report.md`: target BFA-era content/systems catalogue.
- `Dev/playerbots_docs/*`: port plan, API reference, gap audit, future upgrades.

## Module system architecture
- Discovery: any `modules/<dir>/src` folder is a module.
- Loader: `mod-psychobot` -> `Addmod_psychobotScripts()`, generated into `AddModulesScripts()`.
- Static mode: module sources compiled into static `modules` library linked by `worldserver`.
- Dynamic mode: module gets shared library target and worldserver dependency.
- Config: `modules/<mod>/conf/*.conf.dist` copied to `configs/modules`.

## Imported files
- Complete `modules/` folder imported from Psycho_Core.
- File counts: `modules/` = 163 files; `modules/mod-psychobot` = 159 files.
- Copied module docs: `docs/HOW_TO_BUILD_A_MODULE.md`, `docs/HOW_TO_INSTALL_MODULES.md`, `Dev/playerbots_docs/ModuleAPI_Reference.txt`, `Dev/playerbots_docs/FUTURE_MOD_PSYCHOBOT_UPGRADES.txt`.

## mod-psychobot architecture
- Entry: `mod_psychobot.cpp`, `mod_psychobot_loader.cpp`.
- Managers: `PsychobotMgr`, `PsychobotLoginMgr`, `PsychobotFactory`, `PsychobotGroupMgr`, `PsychobotPopulationMgr`, `PsychobotAhBot`.
- Engine: Strategy/Action/Value/Trigger model.
- Class coverage: 12 classes / 36 specs via class strategy folders.
- Commands: `.psychobot add/remove/list/spec/group/follow/stay/attack/cast/strategy/summon/factory/help`.
- SQL: auth RBAC, character strategy persistence, world bot names/text/races pools.

## Core integration status
- Module build integration: added.
- ScriptMgr `AddModulesScripts()` call: added.
- Worldserver link to `modules`: added.
- Complete `mod-psychobot`: imported.
- Socketless session hooks: added/adapted for Psycho_Core.

## Configure result
- CMake configure succeeds and reports `mod-psychobot` under `Module configuration (static)`.
- No build was run.