# Psycho_Core/Psycho API Difference Report

Generated after porting Psycho toolchain, module infrastructure, full `modules/`, and minimal S28 socketless hooks into original Psycho_Core-8.3.7.

## Scope
- Focus: APIs used by `mod-psychobot`: sessions, player, unit, spells, talents, DB2, group/LFG, movement, pets, auction, DB, commands/scripts.
- Static analysis only. CMake configure succeeded, but no C++ compilation/build was run.

## Key result
- CMake detects `mod-psychobot` with `MODULES=static`.
- S28 socketless session APIs missing in original Psycho_Core were ported/adapted.
- Added Psycho_Core-specific 11-argument `WorldSession` overload so Psycho module calls delegate to Psycho_Core native constructor.

## Core hook/symbol presence
| Check | Present |
|---|---:|
| WorldSession::IsBot API | yes |
| WorldSession::SetBot API | yes |
| WorldSession::SetBotRemove API | yes |
| WorldSession::BotLogin API | yes |
| WorldSession 11-arg overload | yes |
| Bot queue bypass | yes |
| Module loader call | yes |
| mod-psychobot CMake message | yes |

## Relevant API file comparison
| File | Bfa lines | Psycho lines | Exact? | Approx first-10k-line overlap |
|---|---:|---:|---:|---:|
| `src/server/game/Server/WorldSession.h` | 2229 | 1903 | no | 0.802 |
| `src/server/game/Server/WorldSession.cpp` | 1608 | 1439 | no | 0.821 |
| `src/server/game/Handlers/CharacterHandler.cpp` | 2701 | 2750 | no | 0.888 |
| `src/server/game/World/World.cpp` | 3653 | 3686 | no | 0.899 |
| `src/server/game/Scripting/ScriptMgr.h` | 1477 | 1223 | no | 0.697 |
| `src/server/game/Scripting/ScriptMgr.cpp` | 3037 | 2519 | no | 0.738 |
| `src/server/game/Entities/Player/Player.h` | 3177 | 2870 | no | 0.824 |
| `src/server/game/Entities/Unit/Unit.h` | 2397 | 2248 | no | 0.798 |
| `src/server/game/Entities/Pet/Pet.h` | 192 | 173 | no | 0.847 |
| `src/server/game/Groups/Group.h` | 424 | 463 | no | 0.819 |
| `src/server/game/Groups/GroupMgr.h` | 61 | 61 | no | 0.957 |
| `src/server/game/DungeonFinding/LFG.h` | 142 | 142 | no | 0.964 |
| `src/server/game/DungeonFinding/LFGMgr.h` | 530 | 498 | no | 0.861 |
| `src/server/game/Spells/SpellMgr.h` | 779 | 790 | no | 0.900 |
| `src/server/game/Spells/SpellInfo.h` | 734 | 710 | no | 0.881 |
| `src/server/game/Spells/SpellHistory.h` | 186 | 177 | no | 0.908 |
| `src/server/game/DataStores/DB2Stores.h` | 430 | 384 | no | 0.865 |
| `src/server/game/DataStores/DB2Structure.h` | 3853 | 3409 | no | 0.835 |
| `src/server/game/DataStores/DB2Manager.h` | missing | missing | - | - |
| `src/server/game/Movement/MotionMaster.h` | 215 | 213 | no | 0.761 |
| `src/server/game/Globals/ObjectMgr.h` | 1932 | 1850 | no | 0.791 |
| `src/server/game/Accounts/AccountMgr.h` | 99 | 98 | no | 0.961 |
| `src/server/game/Chat/Chat.h` | 243 | 200 | no | 0.654 |
| `src/server/game/AuctionHouse/AuctionHouseMgr.h` | 426 | 426 | no | 0.734 |
| `src/server/game/AuctionHouseBot/AuctionHouseBot.h` | 318 | 319 | no | 0.779 |
| `src/server/database/Database/Implementation/CharacterDatabase.h` | 746 | 684 | no | 0.862 |
| `src/server/database/Database/Implementation/LoginDatabase.h` | 198 | 184 | no | 0.834 |
| `src/server/database/Database/Implementation/WorldDatabase.h` | 121 | 120 | no | 0.954 |

## Header/function symbol deltas in major API headers
### `src/server/game/Entities/Player/Player.h`
- Bfa symbols: 1371; Psycho symbols: 1221; common: 1187
- Bfa-only sample: `AddBattlePet, AddBattlePetByCreatureId, AddBattlePetWithSpeciesId, AddChallengeKey, AddCompletedChallenge, AddConversationDelayedTeleport, AddDelayedConversation, AddDelayedTeleport, AddDonateTokenCount, AddGarrisonFollower, AddGarrisonMission, AddGarrisonShipment, AddMovieDelayedAction, AddRunePower, AddToPlayerPetDataStore, Affix, Affix1, Affix2, ApplyWargameItemModifications, AutoUnequip, CalculateCurrencyWeekCap, ChallengeKeyCharded, ChallengeKeyInfo, CompletedChallenge, Copyright, CreateChallengeKey, CreationDate, DeleteFromPlayerPetDataStore, DestroyDonateTokenCount, FinishDay, FinishWeek, ForceCompleteQuest, GetAchievementMgr, GetAdventureQuestID, GetArchaeologyMgr, GetArenaMatchMakerRating, GetArtifactWeapon, GetAverageItemLevelEquipped, GetAverageItemLevelEquippedAndBag, GetBattlePet, GetBattlePetCombatSize, GetBattlePetCombatTeam, GetBattlePetCountForSpecies, GetBattlePetTrapLevel, GetBattlePets, GetBattlegroundRewardCrate, GetBestRatingOfSeason, GetBestRatingOfWeek, GetBgQueueTeam, GetCanUseDonate`
- Psycho-only sample: `AddComboPoints, AddPetAura, ApplyBaseModPctValue, CheckAttackFitToAuraRequirement, ClearComboPoints, ClearQuestSharingInfo, GainSpellComboPoints, GetArenaTeamId, GetArenaTeamIdInvited, GetAverageItemLevel, GetComboPoints, GetPlayerSharingQuest, GetRBGPersonalRating, GetSharedQuestID, HandleBaseModFlatValue, HandlePassiveSpellLearn, LeaveAllArenaTeams, RemovePetAura, SatisfyQuestDependentPreviousQuests, SatisfyQuestDependentQuests, SetArenaTeamIdInvited, SetArenaTeamInfoField, SetBaseModFlatValue, SetBaseModPctValue, SetContestedPvP, SetInArenaTeam, SetModDamageDonePercent, SetQuestSharingInfo, UpdateAllWeaponDependentCritAuras, UpdateBaseModGroup, UpdateDamageDoneMods, UpdateWeaponDependentAuras, UpdateWeaponDependentCritAuras, _LoadArenaTeamInfo`
### `src/server/game/Entities/Unit/Unit.h`
- Bfa symbols: 1018; Psycho symbols: 966; common: 928
- Bfa-only sample: `AddDelayedEvent, AddPetAura, AddSummonedCreature, Anim, ApplyStatBuffMod, ApplyStatPercentBuffMod, CastExpelHarmDamage, CastSpellWithOrientation, CheckPowerProc, Copyright, DeleteThreatList, DistanceCompareOrderPred, DistanceOrderPred, GetAnyUnitListInRange, GetAreaTriggerListWithSpellIDInRange, GetAreatriggerListInRange, GetAttackableUnitListInRange, GetAttackersCount, GetAuraEffectAmount, GetAuraEffectsByMechanic, GetAuraEffectsByTypes, GetBattlePetCompanionNameTimestamp, GetBrawlerGuild, GetChannelObjects, GetConversationListInRange, GetCreature, GetCurrentPetBattle, GetDamageOverLastSeconds, GetDistance, GetDummyAuraEffect, GetEclipsePower, GetEffectiveLevel, GetEffectiveResistChance, GetFriendlyUnitListInRange, GetGameObjectByEntry, GetLastEclipsePower, GetLastUpdatePower, GetModifierValue, GetOwnedAurasByTypes, GetSceneObjectListInRange, GetScheduler, GetStateAnimId, GetSummonList, GetSummonedCreatureByEntry, GetTargetAuraApplications, GetTotalSpellPowerValue, HandleStatModifier, HasMovementForce, HealthWillBeAbovePctHealed, HealthWillBeBelowPctDamaged`
- Psycho-only sample: `ApplyStatPctModifier, CalculateAverageResistReduction, CanInstantCast, CancelSpellMissiles, CheckAttackFitToAuraRequirement, EngageWithTarget, GetFaction, GetFlatModifierValue, GetPctModifierValue, GetThreatManager, HandleStatFlatModifier, IsEngaged, IsEngagedBy, IsFocusing, IsImmuneToAll, IsImmuneToNPC, IsImmuneToPC, IsThreatListEmpty, IsThreatened, IsThreatenedBy, PauseMovement, PropagateSpeedChange, ResumeMovement, SetImmuneToAll, SetImmuneToNPC, SetImmuneToPC, SetInstantCast, SetMainHandWeaponAttackPower, SetOffHandWeaponAttackPower, SetRangedWeaponAttackPower, SetStatFlatModifier, SetStatPctModifier, UpdateAllDamageDoneMods, UpdateAllDamagePctDoneMods, UpdateDamageDoneMods, UpdateDamagePctDoneMods, UpdateStatBuffMod, UpdateUnitMod`
### `src/server/game/Spells/SpellMgr.h`
- Bfa symbols: 104; Psycho symbols: 104; common: 100
- Bfa-only sample: `Copyright, GetSpellInfoStoreSize, _GetSpellInfo, size`
- Psycho-only sample: `ForEachSpellInfo, ForEachSpellInfoDifficulty, operator, void`
### `src/server/game/Spells/SpellInfo.h`
- Bfa symbols: 184; Psycho symbols: 179; common: 177
- Bfa-only sample: `CasterCanTurnDuringCast, Copyright, GetEffectsForDifficulty, HasSameTargets, IsActiveMitigationDamage, IsStatCompatible, IsTargetingLine`
- Psycho-only sample: `GetEffects, size`
### `src/server/game/DataStores/DB2Manager.h`
- Bfa symbols: 0; Psycho symbols: 0; common: 0
- Bfa-only sample: `none`
- Psycho-only sample: `none`
### `src/server/game/Groups/Group.h`
- Bfa symbols: 139; Psycho symbols: 152; common: 129
- Bfa-only sample: `Copyright, FinishGame, GetAverageMMR, GetMaxCountOfRolesForArenaQueue, GetRating, InChallenge, LostAgainst, MemberLost, OfflineMemberLost, WonAgainst`
- Psycho-only sample: `CountRollVote, CountTheRoll, EndRoll, FillPacket, GetInviteeCount, GetItemDisenchantLoot, GetMemberFlags, GetRoll, GroupLoot, MasterLoot, Roll, SendLootAllPassed, SendLootRoll, SendLootRollWon, SendLootRollsComplete, SendLootStartRollToPlayer, SendLooter, empty, getLoot, isRollLootActive, position, setLoot, targetObjectBuildLink`
### `src/server/game/Movement/MotionMaster.h`
- Bfa symbols: 67; Psycho symbols: 62; common: 60
- Bfa-only sample: `Copyright, MoveAwayAndDespawn, MoveBackward, MoveForward, _expireList, h, propagateSpeedChange`
- Psycho-only sample: `MoveFormation, PropagateSpeedChange`
### `src/server/game/Server/WorldSession.h`
- Bfa symbols: 795; Psycho symbols: 659; common: 646
- Bfa-only sample: `AddAuthFlag, Copyright, GetAF, GetBattlePayMgr, GetBattlenetAccountName, HandleAcceptWargameInvite, HandleAdventureJournalOpenQuest, HandleAdventureJournalStartQuest, HandleArtifactAttunePreviewRelic, HandleArtifactAttuneSocketedRelic, HandleAutoBankReagentOpcode, HandleAutoStoreBankReagentOpcode, HandleBattlePayAckFailedResponse, HandleBattlePayConfirmPurchase, HandleBattlePayDistributionAssign, HandleBattlePayPurchaseProduct, HandleBattlePayPurchaseUnkResponse, HandleBattlePayQueryClassTrialResult, HandleBattlePayStartPurchase, HandleBattlePayTrialBoostCharacter, HandleBattlePetClearFanfare, HandleBattlePetDelete, HandleBattlePetDeletePetCheat, HandleBattlePetJournalLock, HandleBattlePetLeaveQueue, HandleBattlePetNameQuery, HandleBattlePetSetSlot, HandleBattlePetUpdateNotify, HandleBattlemasterJoinArenaSkirmish, HandleBattlemasterJoinBrawl, HandleBuyReagentBankOpcode, HandleCancelMasterLootRoll, HandleChallengeModeRequestMapStats, HandleChallengeModeRequestMapStatsOpcode, HandleChallengeModeStart, HandleDepositReagentBankOpcode, HandleDoMasterLootRoll, HandleGarrisonCheckUpgradeable, HandleGarrisonCompleteMission, HandleGarrisonCreateShipmentOpcode, HandleGarrisonGenerateRecruits, HandleGarrisonGetShipmentInfo, HandleGarrisonMissionBonusRoll, HandleGarrisonOpenMissionNpc, HandleGarrisonRecruitFollower, HandleGarrisonRequestClassSpecCategoryInfo, HandleGarrisonResearchTalent, HandleGarrisonSetFollowerInactive, HandleGarrisonStartMission, HandleGarrisonSwapBuildings`
- Psycho-only sample: `GetBattlePetMgr, HandleBattlePetDeletePet, HandleBattlePetSetBattleSlot, HandleBuyStableSlot, HandleGuildBankSwapItems, HandleStablePet, HandleStablePetCallback, HandleStableSwapPet, HandleStableSwapPetCallback, HandleUnstablePet, HandleUnstablePetCallback, SendPetitionSigns, SendStablePetCallback`

## mod-psychobot include/core dependency inventory
- Module C++ source/header files scanned: 145
- Top includes:
  - `string` x44
  - `Player.h` x36
  - `../../engine/AiObjectContext.h` x36
  - `Define.h` x32
  - `Unit.h` x25
  - `../../engine/NamedObjectContext.h` x24
  - `SharedDefines.h` x19
  - `../../engine/Strategy.h` x12
  - `../../PsychobotAIFwd.h` x12
  - `../../engine/Trigger.h` x12
  - `Log.h` x10
  - `../engine/AiObjectContext.h` x10
  - `../PsychobotAIFwd.h` x10
  - `../../engine/ServerFacade.h` x9
  - `../engine/ServerFacade.h` x8
  - `../engine/NamedObjectContext.h` x8
  - `Config.h` x6
  - `ObjectGuid.h` x6
  - `AiObject.h` x6
  - `NextAction.h` x5
  - `PsychobotLoginMgr.h` x4
  - `WorldSession.h` x4
  - `memory` x4
  - `ObjectAccessor.h` x4
  - `DB2Stores.h` x4
  - `PsychobotPopulationMgr.h` x4
  - `set` x4
  - `../engine/Action.h` x4
  - `../engine/Strategy.h` x4
  - `map` x4
  - `Action.h` x4
  - `Event.h` x4
  - `list` x4
  - `ObjectMgr.h` x3
  - `vector` x3
  - `PsychobotFactory.h` x3
  - `Common.h` x3
  - `DatabaseEnv.h` x3
  - `World.h` x3
  - `DB2Structure.h` x3
  - `PsychobotMgr.h` x3
  - `cstdlib` x3
  - `ctime` x3
  - `SpellInfo.h` x3
  - `DBCEnums.h` x3
  - `Trigger.h` x3
  - `Strategy.h` x3
  - `PsychobotAhBot.h` x2
  - `AccountMgr.h` x2
  - `PsychobotGroupMgr.h` x2
  - `CharacterCache.h` x2
  - `unordered_map` x2
  - `PsychobotAiFactory.h` x2
  - `PsychobotTalentMgr.h` x2
  - `PsychobotGearMgr.h` x2
  - `../PsychobotGroupMgr.h` x2
  - `../engine/Engine.h` x2
  - `MotionMaster.h` x2
  - `SpellMgr.h` x2
  - `cctype` x2
  - `cmath` x2
  - `PsychobotSpecRoles.h` x2
  - `functional` x2
  - `PsychobotAIFwd.h` x2
  - `NamedObjectContext.h` x2
  - `ActionNode.h` x2
  - `Queue.h` x2
  - `Multiplier.h` x2
  - `PetActions.h` x2
  - `../engine/Trigger.h` x2

## Spells/talents/DB2 findings
- `PsychobotTalentMgr` uses specialization/talent APIs: `GetPrimarySpecialization`, `SetPrimarySpecialization`, `ActivateTalentGroup`, `LearnTalent`, and DB2 manager talent/specialization lookups.
- Spell path uses `sSpellMgr->GetSpellInfo(spellId, DIFFICULTY_NONE)`, `Player::HasSpell`, `SpellHistory::HasCooldown`, `Unit::CastSpell`, aura iteration, and `SpellInfo::Dispel`.
- `Player.h`, `DB2Manager.h`, `SpellMgr.h`, `SpellInfo.h`, and `SpellHistory.h` are not exact matches with Psycho; compile validation is still required later.
- BFA DB2 reference data in Psycho `Dev/DB2-DATA` was not copied wholesale. Runtime DB2 data still needs normal client extraction/data setup.

## Known remaining risk areas before build approval
- No build has been run, so C++ compile errors may still exist in `mod-psychobot`.
- Highest-risk module calls: `WorldSession` construction/login, `Player::Create`, `Player::LearnTalent`, `ActivateTalentGroup`, group/LFG role calls, auction-house scaffolding, and DB prepared statement enum names.
- S28 socketless hooks are ported/adapted but runtime behavior is untested.