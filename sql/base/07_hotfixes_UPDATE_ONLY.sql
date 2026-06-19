-- ===========================================================================
-- Psycho_Core 8.3.7 - HOTFIXES DATABASE - UPDATE ONLY (base schema NOT included)
-- Target: psycho_hotfixes on 127.0.0.1:3307  (MariaDB 11.8.6)
--
-- IMPORTANT: This is only a PATCH. It assumes the full hotfixes schema
-- (hundreds of DB2-mirroring tables, including `hotfix_data` and
-- `spell_effect` used below) already exists in psycho_hotfixes.
-- Neither DB-3.zip, DB-4.zip, nor the core's sql/base/ contained that
-- base schema - see Errors_report.txt, Session 8, for details.
--
-- DO NOT run this until the hotfixes base schema has been imported,
-- or it will fail with "table doesn't exist" errors.
-- ===========================================================================

/*
**************************
*    BfaCore Reforged    *
**************************
*/

-- Learn Fishing and Fishing skills
DELETE FROM `spell_effect` WHERE `ID`IN (721957,721958);
INSERT INTO `spell_effect` (`ID`, `EffectAura`, `DifficultyID`, `EffectIndex`, `Effect`, `EffectAmplitude`, `EffectAttributes`, `EffectAuraPeriod`, `EffectBonusCoefficient`, `EffectChainAmplitude`, `EffectChainTargets`, `EffectItemType`, `EffectMechanic`, `EffectPointsPerResource`, `EffectPosFacing`, `EffectRealPointsPerLevel`, `EffectTriggerSpell`, `BonusCoefficientFromAP`, `PvpMultiplier`, `Coefficient`, `Variance`, `ResourceCoefficient`, `GroupSizeBasePointsCoefficient`, `EffectBasePoints`, `EffectMiscValue1`, `EffectMiscValue2`, `EffectRadiusIndex1`, `EffectRadiusIndex2`, `EffectSpellClassMask1`, `EffectSpellClassMask2`, `EffectSpellClassMask3`, `EffectSpellClassMask4`, `ImplicitTarget1`, `ImplicitTarget2`, `SpellID`, `VerifiedBuild`) VALUES
(721957, 0, 0, 0, 36, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 271990, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 271617, 35662),
(721958, 0, 0, 1, 36, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 131474, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 271617, 35662);

DELETE FROM `hotfix_data` WHERE `RecordId` IN (721957,721958);
INSERT INTO `hotfix_data` (`Id`, `TableHash`, `RecordId`, `Deleted`, `VerifiedBuild`) VALUES
(271617, 4030871717, 721957, 0, 35662),
(271617, 4030871717, 721958, 0, 35662);
