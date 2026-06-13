-- ============================================================================
--  mod-psychobot - characters DB schema
--  Table: psychobot_strategies
--
--  Persists a bot's master-toggled extra combat strategy overrides (S27,
--  PsychobotDbStore) keyed by the bot's character GUID, so they survive a relog.
--  One row per (bot guid, strategy name) that is currently enabled.
--
--  Apply to the CHARACTERS database. Idempotent (safe to re-run).
--  Psycho_Core - TrinityCore 8.3.7 (BfA 8.3.0).
-- ============================================================================

CREATE TABLE IF NOT EXISTS `psychobot_strategies` (
  `guid`  BIGINT(20)  UNSIGNED NOT NULL              COMMENT 'bot character GUID (low part)',
  `name`  VARCHAR(64)          NOT NULL              COMMENT 'enabled strategy name',
  PRIMARY KEY (`guid`, `name`),
  KEY `idx_psychobot_strategies_guid` (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
  COMMENT='mod-psychobot: per-bot saved (master-toggled) combat strategies';
