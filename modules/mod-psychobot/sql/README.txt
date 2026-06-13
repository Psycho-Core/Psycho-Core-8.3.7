================================================================================
 mod-psychobot - SQL
================================================================================

WHAT TO APPLY
-------------
Required for normal strategy persistence:

  characters/psychobot_strategies.sql   -> apply to the CHARACTERS database.
      Creates `psychobot_strategies` (guid, name). Used by PsychobotDbStore
      to persist each bot's master-toggled extra combat strategies across relogs.
      Idempotent (CREATE TABLE IF NOT EXISTS) - safe to re-run.

Required for in-game `.psychobot` commands:

  auth/psychobot_rbac.sql               -> apply to the AUTH/Login database.
      Creates custom RBAC permission IDs 1200-1215 and links the command tree to
      GM/security role 1+ via permission 194. Without this, the module can build
      but player/GM sessions may fail command permission checks.

Required if S30 auto-spawn / random-bot generation is enabled:

  world/psychobot_names.sql             -> apply to the WORLD database.
      Creates and populates `ai_playerbot_names`, `ai_playerbot_guild_names`,
      and `ai_playerbot_arena_team_names`. PsychobotFactory currently reads
      `ai_playerbot_names` into memory when generating new bot characters.

HOW TO APPLY (manually, until auto-SQL wiring is enabled on your build)
----------------------------------------------------------------------
    mysql -u <user> -p <auth_db>       < auth/psychobot_rbac.sql
    mysql -u <user> -p <characters_db> < characters/psychobot_strategies.sql
    mysql -u <user> -p <world_db>      < world/psychobot_names.sql

OTHER SQL FILES IN THIS MODULE
------------------------------
The module also carries legacy/reference-compatible tables for future expansion
or optional data packs:

  characters/psychobot_random_bots.sql
  characters/psychobot_custom_strategy.sql
  characters/psychobot_db_store.sql
  world/psychobot_texts.sql
  world/psychobot_rpg_races.sql
  world/psychobot_cache.sql
  world/psychobot_indexes.sql

Current C++ code directly references / depends on:
  - auth DB      : RBAC permissions 1200-1215 for `.psychobot` commands
  - characters DB: `psychobot_strategies`
  - world DB     : `ai_playerbot_names` (from psychobot_names.sql)

If the worldserver log reports a missing table, import the matching SQL file.
Otherwise, do not import older external ai_playerbot_*.sql files blindly; see
Dev/Issues/Error_report.txt for the review of redundant/destructive legacy SQL.
================================================================================
