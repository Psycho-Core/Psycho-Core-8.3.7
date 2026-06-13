-- ===========================================================================
--  mod-psychobot - auth DB RBAC permissions
-- ===========================================================================
-- Apply to the AUTH/Login database.
-- Purpose: make the `.psychobot` command tree usable by GM accounts.
-- The CommandScript uses custom permission IDs 1200-1215; without these rows,
-- the commands can compile but fail permission checks at runtime.
-- Idempotent: safe to re-run.

INSERT IGNORE INTO `rbac_permissions` (`id`, `name`) VALUES
(1200, 'Command: psychobot'),
(1201, 'Command: psychobot add'),
(1202, 'Command: psychobot remove'),
(1203, 'Command: psychobot list'),
(1204, 'Command: psychobot spec'),
(1205, 'Command: psychobot group'),
(1206, 'Command: psychobot follow'),
(1207, 'Command: psychobot stay'),
(1208, 'Command: psychobot attack'),
(1209, 'Command: psychobot cast'),
(1210, 'Command: psychobot strategy'),
(1211, 'Command: psychobot factory'),
(1212, 'Command: psychobot factory purge'),
(1213, 'Command: psychobot help'),
(1214, 'Command: psychobot factory spawn'),
(1215, 'Command: psychobot summon');

-- The parent psychobot permission grants the whole psychobot command tree.
INSERT IGNORE INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(1200, 1201),
(1200, 1202),
(1200, 1203),
(1200, 1204),
(1200, 1205),
(1200, 1206),
(1200, 1207),
(1200, 1208),
(1200, 1209),
(1200, 1210),
(1200, 1211),
(1200, 1212),
(1200, 1213),
(1200, 1214),
(1200, 1215);

-- Give the command tree to GM/security role 1 and above by linking it to the
-- existing GM role permission 194. Higher roles inherit 194 through the stock
-- TrinityCore RBAC chain. If you want ordinary player accounts to use bot
-- commands, explicitly grant permission 1200 to those accounts instead.
INSERT IGNORE INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(194, 1200);
