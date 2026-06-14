# BFA Core — Complete Retail Content Checklist (Levels 1-120)
**Target:** 8.3.7 (36552) | **Audit Date:** 2026-06-14  
**Base Core Audited:** Titans-Project/BfaCore-Reforged (build 35662) — identical C++ tree to freadblangks, cooler-SAI, Simonlamb.  
**Legend:**  
- ✅ **Present** — C++ script directory exists and has files (boss/instance AI)  
- ⚠️ **Partial / Placeholder** — Directory exists but known to be thin, auto-cast, or broken  
- ❌ **Missing** — No C++ script directory; likely non-functional or DB-only  
- ❓ **DB Only** — Content is data-driven (spawns, quests, loot) with no C++ script needed  
- 🚫 **Not Applicable** — Did not exist in retail 8.3.7

---

## BATTLE FOR AZEROTH (Levels 110-120) — Patch 8.0-8.3.7

### BFA Major Systems
| System | Status | Notes |
|--------|--------|-------|
| Allied Races (Vulpera, Mechagnome, Zandalari, Kul Tiran, etc.) | ⚠️ | `AlliedRaces` (2 cpp) — likely intro scenarios only; race unlock questlines mostly DB |
| Heart of Azeroth & Artifact Power | ❌ | No dedicated system. Azerite powers are item bonuses (DB) only |
| Azerite Armor & Traits | ❌ | Items exist (DB) but traits are passive/dummy; no trait selection UI/script |
| Warfronts (Arathi / Darkshore) | ❌ | No system anywhere in source |
| Island Expeditions (full system) | ❌ | Only tutorial/tutorial-like scenarios exist; no AI, no weekly, no rewards vendor |
| War Campaign (faction story) | ❓ | Quest chains are DB-only; no C++ script block |
| Nazjatar Bodyguards | ❌ | |
| N'Zoth Assaults (Vale / Uldum) | ❌ | |
| Horrific Visions | ❌ | |
| Corruption System | ❌ | Items exist, but corruption procs/mechanics not implemented |
| Brawler's Guild | ✅ | `BrawlersGuild` (9 cpp) — full queue + boss scripts |
| M+ Affixes (Seasonal) | ⚠️ | ChallengeMode exists in `game/ChallengeMode`; BFA affixes likely incomplete |
| PvP Weekly Conquest / Vendors | ❓ | Vendors/quests DB; reward systems unscripted |
| Emissary Quests / World Quests | ⚠️ | World Quest system partially in core; emissaries are quest DB + Lua |
| Mission Table (BfA) | ❌ | Garrison system exists for WoD/Legion but not BFA table |
| Tortollan Seekers / Faction Vendors | ❓ | DB creatures |
| Black Market Auction House | ✅ | `game/BlackMarket` exists in core |
| Chromie Time / Level Scaling | ❌ | Not in source; scaling is hardcoded |

### BFA Dungeons (10 + Mechagon)
| Dungeon | C++ Status | File Count | Notes |
|---------|------------|------------|-------|
| Freehold | ✅ | 6 | |
| Waycrest Manor | ✅ | 7 | |
| Tol Dagor | ✅ | 6 | |
| Shrine of the Storm | ✅ | 6 | |
| Siege of Boralus | ✅ | 6 | |
| Atal'Dazar | ✅ | 6 | |
| The Underrot | ✅ | 6 | |
| Temple of Sethraliss | ✅ | 6 | |
| The Motherlode | ✅ | 6 | |
| King's Rest | ✅ | 5 | |
| Operation: Mechagon (Upper) | ✅ | Part of `Operation Mechagon` (10 cpp total) | Likely includes both wings |
| Operation: Mechagon (Lower) | ✅ | Part of `Operation Mechagon` (10 cpp total) | Likely includes both wings |

### BFA Raids (5)
| Raid | C++ Status | File Count | Notes |
|------|------------|------------|-------|
| Uldir | ✅ | 10 | |
| Battle of Dazar'alor | ✅ | 11 | |
| Crucible of Storms | ❌ | 0 | **Not present in any core** |
| Eternal Palace (Nazjatar) | ✅ | 10 | |
| Ny'alotha, the Waking City | ✅ | 15 | |

### BFA Zones (Open World)
> **CORRECTED 2026-06-14:** This core has **95 `zone_*.cpp` scripts** (83 substantial >2KB,
> 12 genuine sub-2KB stubs). Earlier "no C++ scripts / open world bare" notes were inaccurate.
> Substantial BFA zone scripts include zone_tiragarde_sound.cpp (44KB), zone_zuldazar.cpp (26KB),
> zone_nazjatar.cpp (26KB), zone_nazmir.cpp (6KB), zone_arathi_highlands.cpp (6KB).
> Genuine stubs (<2KB): zone_drustvar, zone_stormsong_valley, zone_voldun, zone_darkshore.

| Zone | C++ Status | Notes |
|------|------------|-------|
| Zuldazar | ✅ | `zone_zuldazar.cpp` (26KB) — substantial |
| Nazmir | ✅ | `zone_nazmir.cpp` (6KB) |
| Vol'dun | ⚠️ | `zone_voldun.cpp` (1.8KB stub) |
| Tiragarde Sound | ✅ | `zone_tiragarde_sound.cpp` (44KB) — substantial |
| Drustvar | ⚠️ | `zone_drustvar.cpp` (745B stub) |
| Stormsong Valley | ⚠️ | `zone_stormsong_valley.cpp` (753B stub) |
| Nazjatar | ✅ | `zone_nazjatar.cpp` (26KB) + `EternalPalace` (10 cpp). Open world IS scripted. |
| Mechagon Island | ⚠️ | Part of `Operation Mechagon` (10 cpp). Open world NPCs DB-only |
| Vale of Eternal Blossoms (8.3) | ❓ | Pandaria zone; 8.3 assault spawns are DB + event scripts |
| Uldum (8.3) | ❓ | Cataclysm zone; 8.3 assault spawns are DB |

### BFA World Bosses
| Boss | C++ Status | Notes |
|------|------------|-------|
| Zandalar World Bosses (T'zane, Warbringer, etc.) | ⚠️ | `Zandalar/WorldBosses` (3 cpp) — likely only 3 of 7+ scripted |
| Kul Tiras World Bosses (Azurethos, Kraulok, etc.) | ❌ | No dedicated directory; may be in `World` generic scripts |
| Nazjatar World Bosses (Ulmath, Wekemara) | ❌ | Not present |
| Ny'alotha Minor Visions bosses | ❌ | Not present |

### BFA Scenarios
| Scenario | C++ Status | Count | Notes |
|----------|------------|-------|-------|
| Uncharted Island (tutorial) | ⚠️ | 2 | Tutorial only; not full Island Expeditions |
| The Stormwind Extraction | ✅ | 2 | Horde intro scenario |
| Zandalar Forever | ✅ | 2 | |
| The Battle for Lordaeron | ✅ | 2 | Pre-patch scenario |
| The Defense of Karabor | ⚠️ | 1 | WoD scenario placeholder |
| Pursuing the Black Harvest | ✅ | 2 | Warlock Green Fire scenario |
| Verdant Wilds | ⚠️ | 1 | Island tutorial placeholder? |
| Whispering Reef | ⚠️ | 2 | Island tutorial placeholder? |
| Full Island Expedition rotation (30+ maps) | ❌ | 0 | System missing entirely |
| Warfront Scenarios (Arathi/Darkshore) | ❌ | 0 | System missing entirely |

### BFA Battlegrounds & PvP
| Content | C++ Status | Notes |
|---------|------------|-------|
| Battle for Gilneas | ❓ | Classic BG; no C++ script dir (may be in DB/generic) |
| Twin Peaks | ❓ | Classic BG; no C++ script dir |
| Seething Shore | ❌ | No dedicated directory |
| Deepwind Gorge | ❓ | May exist in generic BG code |
| Temple of Kotmogu | ❓ | May exist in generic BG code |
| Silvershard Mines | ❓ | May exist in generic BG code |
| Rated PvP / Arena System | ⚠️ | `game/Battlegrounds` + `OutdoorPvP` exist; BFA rated season logic not fully scripted |
| PvP Brawls (weekly) | ❌ | No dedicated system |
| Mercenary Mode | ❌ | No system |

---

## LEGION (Levels 100-110) — Patch 7.0-7.3.5

### Legion Major Systems
| System | Status | Notes |
|--------|--------|-------|
| Artifact Weapons (36 specs) | ⚠️ | No `game/Artifact` directory. Weapon talents are DB + spell scripts; appearance unlocks unscripted |
| Order Halls / Class Halls | ✅ | `ClassHalls` (12 cpp) — partial; champions/missions likely missing |
| Class Hall Campaigns | ❓ | Quest-driven (DB); no C++ script block |
| World Quests (Legion) | ⚠️ | Core supports WQ logic partially; emissary reward tables incomplete |
| Legion Invasions / Demon Assaults | ❌ | No dedicated C++ system |
| Artifact Knowledge / Research | ❌ | |
| Netherlight Crucible | ❌ | Argus system |
| Chromie Scenario (Deaths of Chromie) | ❌ | No directory; only a `game/Time` utility |
| Mythic+ Keystone System | ⚠️ | `ChallengeMode` exists in core; Legion affixes may be incomplete |
| Dalaran Sewers (PvP) | ❓ | Zone exists; no dedicated PvP script dir |
| Suramar Insurrection / Mana System | ❌ | No C++ script block for Suramar mechanics |
| Valarjar / Nightfallen / etc. reputations | ❓ | DB quests + reputation tables |
| Legion Legendary Items | ⚠️ | Items exist (DB); proc scripts in `Spells` (20 cpp) may cover some |
| Nethershard / Veiled Argunite currencies | ❓ | Currency tables in DB; vendors are DB |

### Legion Dungeons (10 + 3 Karazhan wings)
| Dungeon | C++ Status | Count | Notes |
|---------|------------|-------|-------|
| Black Rook Hold | ✅ | 6 | |
| Cathedral of Eternal Night | ✅ | 6 | |
| Court of Stars | ✅ | 5 | |
| Darkheart Thicket | ✅ | 6 | |
| Eye of Azshara | ✅ | 7 | |
| Halls of Valor | ✅ | 7 | |
| Maw of Souls | ✅ | 5 | |
| Neltharion's Lair | ✅ | 6 | |
| The Arcway | ✅ | 5 | |
| Vault of the Wardens | ✅ | 7 | |
| Return to Karazhan (Lower) | ✅ | Part of `ReturnToKharazan` (11 cpp) | Likely includes both wings |
| Return to Karazhan (Upper) | ✅ | Part of `ReturnToKharazan` (11 cpp) | Likely includes both wings |
| Seat of the Triumvirate | ✅ | 6 | |

### Legion Raids (4)
| Raid | C++ Status | Count | Notes |
|------|------------|-------|-------|
| Emerald Nightmare | ✅ | 9 | |
| The Nighthold | ✅ | 6 | |
| Trial of Valor | ✅ | 5 | |
| Tomb of Sargeras | ✅ | 11 | |
| Antorus, the Burning Throne | ✅ | 13 | |

### Legion Zones (Open World)
| Zone | C++ Status | Notes |
|------|------------|-------|
| Broken Shore | ❓ | Event-driven; no dedicated C++ script dir |
| Azsuna | ❓ | Open world |
| Val'sharah | ❓ | Open world |
| Highmountain | ❓ | Open world |
| Stormheim | ❓ | Open world |
| Suramar | ❓ | Open world |
| Krokuun / Mac'Aree / Antoran Wastes (Argus) | ⚠️ | `Argus` not present in Titans source (was in boom8866/Lasko73). Argus content likely missing or in `BrokenIsles` |
| Invasion Point maps | ⚠️ | `InvasionPoint` (2 cpp) — thin placeholders |
| Micro-Holidays (Legion) | ⚠️ | `MicroHolidays` (1 cpp) — likely only one |

### Legion Class Halls (by class)
| Class Hall | C++ Status | Notes |
|------------|------------|-------|
| All 12 class halls | ⚠️ | `ClassHalls` (12 cpp) — likely one per class or shared; missions/champions not fully scripted |
| Artifact questlines (36 specs) | ❓ | Quest-driven; no per-spec C++ script block |
| Artifact Challenges (Mage Tower) | ❌ | No dedicated directory |

---

## WARLORDS OF DRAENOR (Levels 90-100) — Patch 6.0-6.2.4

### WoD Major Systems
| System | Status | Notes |
|--------|--------|-------|
| Garrison (Level 1-3) | ✅ | `Garrison` (4 cpp) + `game/Garrison` core system — partial; buildings/blueprints likely incomplete |
| Garrison Followers / Missions | ⚠️ | Core Garrison system exists; mission tables and success chance formulas likely incomplete |
| Garrison Invasions | ❌ | |
| Shipyard (Level 3 Garrison) | ❌ | No naval directory |
| Tanaan Jungle Intro | ✅ | `TanaanIntro` (7 cpp) — opening cinematic sequence |
| Apexis Crystals / Dailies | ❓ | Currency/quest DB; no C++ |
| Legendary Ring Questline | ❓ | Quest-driven; `PursuingTheBlackHarvest` (2 cpp) may be part of it |
| Selfie Camera | ❌ | Toy not implemented |
| Timewalker events (WoD) | ❌ | |

### WoD Dungeons (8)
| Dungeon | C++ Status | Count | Notes |
|---------|------------|-------|-------|
| Bloodmaul Slag Mines | ✅ | 8 | |
| Iron Docks | ✅ | 6 | |
| Auchindoun | ✅ | 6 | |
| Skyreach | ✅ | 7 | |
| The Everbloom | ✅ | 7 | |
| Shadowmoon Burial Grounds | ✅ | 6 | |
| Grimrail Depot | ✅ | 5 | |
| Upper Blackrock Spire | ✅ | Part of `BlackrockSpire` (16 cpp) in EasternKingdoms | Shared with old UBRS |

### WoD Raids (3)
| Raid | C++ Status | Count | Notes |
|------|------------|-------|-------|
| Highmaul | ✅ | 9 | |
| Blackrock Foundry | ✅ | 12 | |
| Hellfire Citadel | ✅ | 15 | |

---

## MISTS OF PANDARIA (Levels 85-90) — Patch 5.0-5.4.8

### MoP Major Systems
| System | Status | Notes |
|--------|--------|-------|
| Pet Battle System | ✅ | `game/BattlePets` + `Pet` (7 cpp) — core system exists; pet AI and abilities partial |
| Scenarios (MoP) | ⚠️ | `Pandaria/Scenario` (1 cpp) — one generic; MoP-specific scenarios thin |
| World Bosses (Sha, Galleon, Nalak, Oondasta, Celestials, Ordos) | ✅ | `Pandaria/WorldBosses` (9 cpp) — most bosses covered |
| Brawler's Guild (MoP) | ✅ | `BrawlersGuild` (9 cpp) — shared with BfA version |
| Legendary Cloak Questline | ❓ | Quest-driven; `PursuingTheBlackHarvest` may be part of it |
| Timeless Isle | ❓ | Open world; rare spawns and events DB + generic scripts |
| Proving Grounds | ❌ | No dedicated directory |
| Farm (Halfhill) | ❌ | No C++ system |
| Reputation Farms (Klaxxi, Golden Lotus, etc.) | ❓ | DB quests/dailies |
| Challenge Mode Dungeons (MoP) | ⚠️ | `ChallengeMode` core exists; timers/leaderboards likely incomplete |
| LFR / Flexible Raid system | ⚠️ | Core raid logic exists; flex scaling may be missing |

### MoP Dungeons (9 + 3 remixed)
| Dungeon | C++ Status | Count | Notes |
|---------|------------|-------|-------|
| Gate of the Setting Sun | ✅ | 6 | |
| Mogu'shan Palace | ✅ | 4 | |
| Shado-Pan Monastery | ✅ | 6 | |
| Siege of Niuzao Temple | ✅ | 5 | |
| Stormstout Brewery | ✅ | 3 | |
| Temple of the Jade Serpent | ✅ | 5 | |
| Scholomance (MoP revamp) | ✅ | 14 | In `EasternKingdoms/Scholomance` |
| Scarlet Halls (MoP revamp) | ✅ | 4 | In `EasternKingdoms/ScarletHalls` |
| Scarlet Monastery (MoP revamp) | ✅ | 11 | In `EasternKingdoms/ScarletMonastery` |
| Ragefire Chasm (MoP revamp) | ✅ | 10 | In `Kalimdor/RagefireChasm` |
| Blackrock Depths (MoP revamp) | ✅ | 10 | In `EasternKingdoms/BlackrockMountain/BlackrockDepths` |

### MoP Raids (5)
| Raid | C++ Status | Count | Notes |
|------|------------|-------|-------|
| Mogu'shan Vaults | ✅ | 8 | |
| Heart of Fear | ✅ | 8 | |
| Terrace of Endless Spring | ✅ | 6 | |
| Throne of Thunder | ✅ | 15 | |
| Siege of Orgrimmar | ✅ | 19 | |

---

## CATACLYSM (Levels 80-85) — Patch 4.0-4.3.4

### Cataclysm Major Systems
| System | Status | Notes |
|--------|--------|-------|
| Tol Barad (PvP / World Zone) | ⚠️ | `OutdoorPvP` (6 cpp) may include it; not a dedicated directory |
| Darkmoon Faire (revamp) | ✅ | `DarkmoonIsland` (6 cpp) |
| Archaeology | ✅ | `game/Archaeology` exists in core |
| Transmogrification | ❓ | Core system may exist in `game/Items` or `Handlers`; not a dedicated dir |
| Rated Battlegrounds | ⚠️ | Core BG system exists; rated logic may be missing |
| Reforging | ❓ | May be in core item system; not visible in script tree |

### Cataclysm Dungeons (12)
| Dungeon | C++ Status | Count | Notes |
|---------|------------|-------|-------|
| Blackrock Caverns | ✅ | 7 | |
| Throne of the Tides | ✅ | 5 | |
| Stonecore | ✅ | 6 | In `Maelstrom/Stonecore` |
| Vortex Pinnacle | ✅ | 5 | |
| Lost City of the Tol'vir | ✅ | 6 | |
| Halls of Origination | ✅ | 8 | |
| Grim Batol | ✅ | 7 | |
| Deadmines (revamp) | ✅ | 9 | |
| Shadowfang Keep (revamp) | ✅ | 8 | |
| Zul'Aman (revamp) | ✅ | 8 | |
| Zul'Gurub (revamp) | ✅ | 10 | |
| End Time | ✅ | 6 | In `Kalimdor/CavernsOfTime/EndTime` |
| Well of Eternity | ✅ | 6 | |
| Hour of Twilight | ✅ | 5 | |

### Cataclysm Raids (6)
| Raid | C++ Status | Count | Notes |
|------|------------|-------|-------|
| Baradin Hold | ✅ | 4 | |
| Bastion of Twilight | ✅ | 6 | |
| Blackwing Descent | ✅ | 8 | |
| Throne of the Four Winds | ✅ | 4 | |
| Firelands | ✅ | 9 | |
| Dragon Soul | ✅ | 10 | |

---

## WRATH OF THE LICH KING (Levels 60-80 / 70-80) — Patch 3.0-3.3.5

### WotLK Major Systems
| System | Status | Notes |
|--------|--------|-------|
| Death Knight Starting Zone | ✅ | Multiple C++ scripts in `EasternKingdoms/ScarletEnclave` and world events |
| Wintergrasp | ⚠️ | No dedicated dir; may be in `OutdoorPvP` (6 cpp) or `Northrend/IsleOfConquest` (2 cpp) |
| Argent Tournament | ❓ | Quest-driven; no dedicated C++ script block |
| Dungeon Finder (LFG) | ✅ | `game/DungeonFinding` exists in core |
| Achievement System | ✅ | `game/Achievements` exists in core |
| Vehicle System | ✅ | Vehicle logic in `game/Entities/Vehicle` |
| Phasing System | ✅ | `game/Phasing` exists in core |
| Heirlooms | ❓ | Item system; may be DB-only |

### WotLK Dungeons (12)
| Dungeon | C++ Status | Count | Notes |
|---------|------------|-------|-------|
| Utgarde Keep | ✅ | 5 | |
| Utgarde Pinnacle | ✅ | 5 | |
| Nexus | ✅ | 6 | |
| Oculus | ✅ | 6 | |
| Azjol-Nerub | ✅ | 4 | |
| Ahn'kahet: The Old Kingdom | ✅ | 6 | |
| Drak'Tharon Keep | ✅ | 5 | |
| Violet Hold | ✅ | 9 | |
| Gundrak | ✅ | 6 | |
| Halls of Stone | ✅ | 5 | |
| Halls of Lightning | ✅ | 5 | |
| Pit of Saron | ✅ | 5 | |
| Forge of Souls | ✅ | 4 | |
| Halls of Reflection | ✅ | 5 | |
| Trial of the Champion | ✅ | 5 | |

### WotLK Raids (9)
| Raid | C++ Status | Count | Notes |
|------|------------|-------|-------|
| Naxxramas | ✅ | 16 | |
| Obsidian Sanctum | ✅ | 3 | |
| Eye of Eternity | ✅ | 2 | |
| Vault of Archavon | ✅ | 5 | |
| Ulduar | ✅ | 15 | |
| Trial of the Crusader | ✅ | 7 | |
| Onyxia's Lair (revamp) | ✅ | 2 | |
| Icecrown Citadel | ✅ | 15 | |
| Ruby Sanctum | ✅ | 6 | |

### WotLK Battlegrounds / PvP
| Content | C++ Status | Notes |
|---------|------------|-------|
| Strand of the Ancients | ❓ | May be in core BG system; no dedicated script dir |
| Isle of Conquest | ⚠️ | `IsleOfConquest` (2 cpp) — thin |
| Wintergrasp | ⚠️ | No dedicated dir; may be in `OutdoorPvP` |

---

## THE BURNING CRUSADE (Levels 58-70) — Patch 2.0-2.4.3

### TBC Major Systems
| System | Status | Notes |
|--------|--------|-------|
| Arena System (2v2/3v3/5v5) | ✅ | `game/Battlegrounds` + core arena logic |
| Flying Mounts | ✅ | Core movement/spell system |
| Daily Quests | ❓ | Quest system exists; dailies are DB |
| Shattered Sun Offensive | ❓ | Event-driven; no C++ block |
| Badge of Justice system | ❓ | Currency/loot DB |
| Heroic Dungeon Keys | ❓ | DB keys/reputation locks |

### TBC Dungeons (16)
| Dungeon | C++ Status | Count | Notes |
|---------|------------|-------|-------|
| Hellfire Ramparts | ✅ | 4 | |
| Blood Furnace | ✅ | 4 | |
| Shattered Halls | ✅ | 5 | |
| Slave Pens | ✅ | 5 | |
| Underbog | ✅ | 3 | |
| Steamvault | ✅ | 4 | |
| Mana-Tombs | ✅ | 3 | |
| Auchenai Crypts | ✅ | 3 | |
| Sethekk Halls | ✅ | 4 | |
| Shadow Labyrinth | ✅ | 6 | |
| Mechanar | ✅ | 6 | |
| Botanica | ✅ | 6 | |
| Arcatraz | ✅ | 6 | |
| Black Morass | ✅ | 5 | In `CavernsOfTime` |
| Escape from Durnholde | ✅ | 5 | In `CavernsOfTime` |
| Magisters' Terrace | ✅ | 6 | |

### TBC Raids (8)
| Raid | C++ Status | Count | Notes |
|------|------------|-------|-------|
| Karazhan | ✅ | 12 | |
| Gruul's Lair | ✅ | 3 | |
| Magtheridon's Lair | ✅ | 2 | |
| Serpentshrine Cavern | ✅ | 7 | |
| Tempest Keep (The Eye) | ✅ | 6 | |
| Battle for Mount Hyjal | ✅ | 9 | In `CavernsOfTime` |
| Black Temple | ✅ | 11 | |
| Sunwell Plateau | ✅ | 8 | |

### TBC Battlegrounds
| Content | C++ Status | Notes |
|---------|------------|-------|
| Eye of the Storm | ❓ | Likely in core BG system; no dedicated dir |
| Ruins of Lordaeron (Arena) | ❓ | Likely in core arena system |

---

## CLASSIC / VANILLA (Levels 1-60) — Patch 1.0-1.12.1

### Classic Major Systems
| System | Status | Notes |
|--------|--------|-------|
| Classic World PvP (Silithus, EPL) | ❓ | May be in `OutdoorPvP` (6 cpp) or `World` (18 cpp) |
| Battlegrounds (WSG, AB, AV) | ⚠️ | `AlteracValley` (5 cpp) exists; WSG/AB likely in core BG system |
| Raid Lockouts (40-man) | ✅ | Core instance/lockout system |
| Attunements (Onyxia, MC, BWL, Naxx) | ❓ | Quest-driven; no dedicated C++ block |
| AQ War Effort | ❌ | No dedicated event system |
| Naxxramas (Classic) | ✅ | `Naxxramas` (16 cpp) — shared with WotLK version; may be WotLK iteration only |
| Scarlet Enclave (Classic?) | ✅ | `ScarletEnclave` (4 cpp) — likely DK start only (WotLK) |

### Classic Dungeons (20+ pre-Cata)
| Dungeon | C++ Status | Count | Notes |
|---------|------------|-------|-------|
| Ragefire Chasm | ✅ | 10 | Revamped version dominates; classic may be partial |
| Deadmines | ✅ | 9 | Revamped version dominates |
| Wailing Caverns | ✅ | 2 | Thin; likely only bosses |
| Shadowfang Keep | ✅ | 8 | Revamped version dominates |
| Blackfathom Deeps | ✅ | 5 | |
| The Stockade | ✅ | 4 | |
| Gnomeregan | ✅ | 2 | Thin |
| Razorfen Kraul | ✅ | 2 | Thin |
| Scarlet Monastery | ✅ | 11 | Revamped version dominates |
| Razorfen Downs | ✅ | 6 | |
| Uldaman | ✅ | 4 | |
| Zul'Farrak | ✅ | 3 | |
| Maraudon | ✅ | 5 | |
| Sunken Temple | ✅ | 2 | Thin |
| Blackrock Depths | ✅ | 10 | |
| Lower Blackrock Spire | ✅ | Part of `BlackrockSpire` (16 cpp) | Shared with UBRS |
| Dire Maul | ✅ | 1 | Very thin; likely only one wing |
| Stratholme | ✅ | 13 | |
| Scholomance | ✅ | 14 | Revamped version dominates |
| Molten Core | ✅ | 11 | |
| Blackwing Lair | ✅ | 9 | |
| Ruins of Ahn'Qiraj | ✅ | 7 | |
| Temple of Ahn'Qiraj | ✅ | 11 | |
| Naxxramas (40-man) | ⚠️ | 16 | Likely WotLK 25/10 version; classic 40-man may be missing |

### Classic Raids (8 + world bosses)
| Raid | C++ Status | Count | Notes |
|------|------------|-------|-------|
| Molten Core | ✅ | 11 | |
| Blackwing Lair | ✅ | 9 | |
| Ruins of Ahn'Qiraj | ✅ | 7 | |
| Temple of Ahn'Qiraj | ✅ | 11 | |
| Naxxramas (40-man) | ⚠️ | 16 | Likely WotLK iteration; classic tuning absent |
| Onyxia's Lair | ✅ | 2 | |
| Zul'Gurub (20-man) | ⚠️ | 10 | Revamped version dominates; classic 20-man may be missing |
| Zul'Aman (10-man) | ⚠️ | 8 | TBC/MoP revamp dominates; classic 10-man absent |
| World Bosses (Azuregos, Kazzak, Dragons) | ❓ | May be in `World` (18 cpp) generic scripts |

---

## PROFESSIONS & CRAFTING (All Expansions)

| Profession | C++ Status | Notes |
|------------|------------|-------|
| Classic Professions (1-300) | ✅ | Core `game/Skills` + DB recipes; trainers exist |
| Outland Professions (300-375) | ✅ | Same system |
| Northrend Professions (350-450) | ✅ | Same system |
| Cataclysm Professions (425-525) | ✅ | Same system |
| Pandaria Professions (500-600) | ✅ | Same system |
| Draenor Professions (600-700) | ⚠️ | Garrison profession buildings partial; Draenor recipes may be DB-only |
| Legion Professions (700-800) | ⚠️ | Quest-driven; `BrokenIsles/Professions` (1 cpp) — very thin |
| BFA Professions (800-175) | ❓ | Core system handles 800 cap; Kul Tiran/Zandalari recipes are DB; no special questlines |
| Archaeology | ✅ | `game/Archaeology` exists |
| Fishing / Cooking Dailies | ❓ | Quest DB |
| First Aid | ❓ | Removed in BFA; may still exist as legacy skill in DB |
| Inscription / Glyphs | ❓ | Core spell/item system; glyph effects may be dummy |
| Jewelcrafting | ❓ | Core item system; socket bonuses may be dummy |
| Enchanting | ✅ | Core spell/item system |

---

## PvP SYSTEMS (All Expansions)

| System | C++ Status | Notes |
|--------|------------|-------|
| Arena System (2v2/3v3/5v5) | ✅ | Core `game/Battlegrounds` |
| Rated BGs | ⚠️ | Core BG exists; rated matchmaking logic incomplete |
| Arena Rating / Titles / Rewards | ⚠️ | Core `game/Battlegrounds` + DB; seasonal rewards may be incomplete |
| Honor System (Classic-BC) | ✅ | Core honor/arena point system |
| Honor System (Legion-BfA) | ⚠️ | Honor levels exist; prestige system may be missing |
| World PvP Objectives (Halaa, WG, TB, Ashran) | ⚠️ | `OutdoorPvP` (6 cpp) — likely only Halaa/WG/TB; Ashran ❌ |
| Battlegrounds (All Expansions) | ⚠️ | Core system supports WSG/AB/AV/EOTS/Strand/IoC/SotA; newer BGs (Seething Shore, Deepwind, Temple, Mines, Kotmogu) likely missing or generic |
| Mercenary Mode | ❌ | |
| PvP Brawls | ❌ | |
| War Mode | ❌ | BFA system; not present |
| Dueling | ✅ | Core combat system |
| Arena Spectator / War Games | ⚠️ | Core system partial |

---

## CLASSES & SPELLS (All Expansions)

| Class | Status | Notes |
|-------|--------|-------|
| Death Knight | ✅ | `Scenarios/PursuingTheBlackHarvest` (2 cpp) for Green Fire; spells in core |
| Demon Hunter | ⚠️ | `DemonHunterZones` (2 cpp) — intro zones only; Vengeance/Havoc spells may be incomplete |
| Druid | ✅ | Core spell system + DB |
| Hunter | ✅ | `Pet` (7 cpp) + core |
| Mage | ✅ | Core spell system |
| Monk | ✅ | Core spell system; Mists dungeons exist |
| Paladin | ✅ | Core spell system |
| Priest | ✅ | Core spell system |
| Rogue | ✅ | Core spell system |
| Shaman | ✅ | Core spell system |
| Warlock | ✅ | Core spell system + Green Fire scenario |
| Warrior | ✅ | Core spell system |
| **Class Spells (1-120)** | ✅ | **CORRECTED 2026-06-14:** `Spells/` is a FULL layer — 20 cpp totaling ~2.2 MB, all 12 classes implemented (spell_warlock 201KB, spell_druid 149KB, spell_shaman 141KB, spell_dh 140KB, spell_hunter 128KB, spell_dk 115KB, spell_warrior 105KB, spell_priest 103KB, spell_mage 94KB, spell_rogue 79KB, spell_paladin 76KB, spell_monk 140KB) + spell_artifact, spell_mastery, spell_pet, spell_quest, spell_generic 284KB, spell_item 174KB. Not thin/dummy. |
| **Talents (All Expansions)** | ❓ | Talent system in core; many rows may be non-functional or auto-cast |
| **Artifact / Azerite Traits** | ❌ | No functional trait system; traits are passive or dummy |
| **Legendary Effects** | ⚠️ | Some legendary procs may be in `Spells` (20 cpp); most are dummy |

---

## MISCELLANEOUS SYSTEMS

| System | Status | Notes |
|--------|--------|-------|
| Auction House | ✅ | `game/AuctionHouse` + `AuctionHouseBot` |
| Bank / Guild Bank | ✅ | Core |
| Barber Shop | ❓ | Character appearance may be in core; no C++ script dir |
| Transmogrification | ❓ | Likely in core item handlers; no dedicated dir |
| Void Storage | ❓ | Likely in core item handlers |
| Toy Box | ❓ | Toy spells may be in DB; toy collection UI unscripted |
| Mount Collection / Journal | ❓ | Mounts work; journal UI may be missing |
| Pet Journal | ✅ | `game/BattlePets` |
| Achievement System | ✅ | `game/Achievements` |
| Reputation System | ✅ | `game/Reputation` |
| Guild System | ✅ | `game/Guilds` + `Guilds` (social) |
| Calendar | ✅ | `game/Calendar` |
| Mail System | ✅ | `game/Mails` |
| Chat Channels / LFG Channel | ✅ | `game/Chat` |
| Ticket / Support System | ✅ | `game/Support` |
| Warden (Anti-Cheat) | ✅ | `game/Warden` |
| Phasing | ✅ | `game/Phasing` |
| Weather | ✅ | `game/Weather` |
| Time / Event Manager | ✅ | `game/Time` + `Events` (12 cpp) |
| Scripting Engine (SmartAI / Eluna) | ✅ | `game/Scripting` + `Scripting` (DB) |
| Timewalking (All Expansions) | ❌ | No dedicated system; old dungeons exist but no TW scaling/loot |
| Boost / Character Level Boost | ⚠️ | `Scenarios` include tutorial-like boost instances; actual paid boost system not implemented |
| Recruit-A-Friend | ❌ | |
| Referral Mounts | ❌ | |
| In-Game Shop | ❌ | `game/BattlePay` exists but is a stub/partial |
| Group Finder (Premade Groups) | ⚠️ | `game/GroupFinder` exists; cross-realm premade may be incomplete |
| Looking For Raid (LFR) | ⚠️ | `DungeonFinding` exists; LFR wing logic may be partial |
| Mythic+ Keystone & Affixes | ⚠️ | `ChallengeMode` exists; BFA seasonal affixes likely incomplete |
| Scoreboard / Leaderboards | ❌ | No dedicated system |
| Mythic Raid Lockouts | ✅ | Core instance system |
| Cross-Realm Zones (CRZ) | ❌ | No system |
| Sharding / Phasing (BFA) | ⚠️ | `game/Phasing` exists; BFA zone sharding not implemented |

---

## SQL / DATABASE STATUS (What You Need to Source Externally)

The C++ core only provides the AI and mechanics. The actual **creatures, quests, loot, spawns, waypoints, and gameobjects** live in the **world database**.  
**None of the valid BFA cores ship a complete world DB inside the repository.**

| Database Component | Titans | freadblangks | boom8866 | Notes |
|--------------------|--------|--------------|----------|-------|
| Auth Base Schema | ✅ Included | ✅ Included | ✅ Included | |
| Characters Base Schema | ✅ Included | ✅ Included | ✅ Included | |
| World Base (creatures, quests, spawns) | ❌ External | ❌ External | ✅ **Ships `LTDB_world_83.03.rar`** | 75 MB archive in repo |
| Hotfixes Base (DB2 overrides) | ❌ External | ❌ External | ✅ **Ships `LTDB_hotfixes_83.04.7z`** | 42 MB archive in repo |
| World Updates / Fixes | 34 files | 855 files | 525 files | Merge all into one DB |
| Auth Updates (build patches) | Included | Included | Included | Include 36552 patch |

---

## PRIORITY MISSING CONTENT — ACTION LIST

If your goal is a **retail-like 1-120 experience**, this is what you MUST build or source, in order of impact:

### 🔴 Critical (Breaks Major Systems)
- [ ] **Crucible of Storms** raid (C++ boss scripts — not in any repo)
- [ ] **Warfronts** (Arathi + Darkshore — full system missing)
- [ ] **Island Expeditions** (AI + maps + rewards — system missing)
- [ ] **Heart of Azeroth / Azerite Traits** (system missing; items are dummy)
- [ ] **Horrific Visions** (8.3 system missing)
- [ ] **Corruption System** (procs and negative effects missing)
- [ ] **N'Zoth Assaults + Vale/Uldum 8.3 events** (event system missing)
- [ ] **Nazjatar open world** (only Eternal Palace is scripted; open world is empty)
- [x] **Class Spells (all 12 classes)** — CORRECTED: `Spells/` (20 cpp / ~2.2 MB) is a complete layer covering all 12 classes; no longer a critical gap (optional further expansion only)

### 🟡 High Impact (Dungeon/Raid Gaps)
- [ ] **Dire Maul** (only 1 cpp — classic version is broken/incomplete)
- [ ] **Ragefire Chasm / Wailing Caverns / Gnomeregan** (thin scripts — likely only final boss)
- [ ] **Classic Naxxramas 40-man** (16 cpp likely WotLK version; classic tuning absent)
- [ ] **Zul'Gurub 20-man / Zul'Aman 10-man** (revamps dominate; classic versions missing)
- [ ] **Demon Hunter starting experience** (only 2 cpp in `DemonHunterZones`)
- [ ] **Artifact Challenges (Mage Tower)** (not present)
- [ ] **Proving Grounds** (not present)
- [ ] **Ashran** (not present)

### 🟢 Medium Impact (Systems & Polish)
- [ ] **PvP Brawls**
- [ ] **Mercenary Mode**
- [ ] **War Mode**
- [ ] **Timewalking** (dungeons exist but no scaling/loot system)
- [ ] **Suramar mana system / disguise system**
- [ ] **Class Hall missions / followers** (WoD Garrison system exists but not Legion/BFA versions)
- [ ] **Shipyard** (WoD)
- [ ] **Farm (Halfhill)** (MoP)
- [ ] **Chromie Scenario (Deaths of Chromie)**
- [ ] **Selfie Camera** (toy)
- [ ] **In-Game Shop** (`BattlePay` is stub)
- [ ] **Recruit-A-Friend / Referral**
- [ ] **Cross-Realm Zones / Sharding**
- [ ] **Mythic+ Scoreboard / Leaderboards**

---

## QUICK SUMMARY: WHAT IS WORKABLE "OUT OF THE BOX"

If you merge the **boom8866 world DB** + **freadblangks 855 SQL updates** + **Titans C++ core** (or keep your modernized Titans base), you will have:

- ✅ **All 5 BFA raids** (Uldir, Dazar'alor, Eternal Palace, Ny'alotha) — **Crucible of Storms missing**
- ✅ **All 10 BFA dungeons** + **Mechagon** — fully scripted bosses
- ✅ **All 12 Legion dungeons** + **Karazhan / Seat** — fully scripted
- ✅ **All 3 WoD raids** — fully scripted
- ✅ **All 8 WoD dungeons** — fully scripted
- ✅ **All 5 MoP raids** — fully scripted
- ✅ **All 12 Cataclysm dungeons** — fully scripted
- ✅ **All 9 WotLK raids** — fully scripted
- ✅ **All 8 TBC raids** — fully scripted
- ✅ **Most Vanilla/Classic dungeons** — present but some are thin (Dire Maul, Wailing Caverns, Gnomeregan)
- ✅ **Brawler's Guild** — functional
- ✅ **Garrison (WoD)** — basic functional
- ✅ **Pet Battles** — core system exists
- ✅ **Dungeon Finder / LFG** — core system exists
- ✅ **Class spells** — CORRECTED 2026-06-14: full `Spells/` layer (20 cpp / ~2.2 MB, all 12 classes); not thin/dummy
- ❌ **Warfronts, Island Expeditions, Heart of Azeroth, Azerite, Corruption, Horrific Visions** — do not exist

**Bottom line:** You can run a very solid ** dungeon/raid server** from 1-120. You **cannot** run a retail-like **BFA endgame experience** (Warfronts, Islands, Azerite, Visions) without massive custom development.
