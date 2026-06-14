# BFA & Legion — Custom Scripts (src/server/scripts/Custom) & SQL Checklist
**Rewritten:** 2026-06-14 · For: bfa-core (BFA 8.3.7.35662) + Legion 7.3.5
**Scope:** ONLY genuine BFA/Legion-core `src/server/scripts/Custom/` content + matching SQL.
**Link status:** ✅ verified live 2026-06-14

> ⚠️ DELIBERATELY EXCLUDED: AzerothCore (3.3.5 WotLK) modules. They DO NOT work on a BFA core —
> wrong C++ API, wrong DB2/hotfix layer, wrong SQL schema. Removed from this list entirely.
> Everything below is from REAL Legion/BFA TrinityCore-lineage cores (same family as yours).

---

## METHOD
I cloned the actual BFA/Legion cores and listed the real files in each core's
`src/server/scripts/Custom/` (and any custom SQL). These are copy/port candidates that already
target the BFA/Legion API — far closer to drop-in for your bfa-core than anything WotLK.

---

## A) BFA 8.3.7 CORES — their `src/server/scripts/Custom/` contents

### 1. devovh/Shadowlands_Dekk-Core  (richest BFA-family Custom set — 21 files) ✅
https://github.com/devovh/Shadowlands_Dekk-Core
Path: `src/server/scripts/DekkCore/Custom/` and `src/server/scripts/Custom/`
```
custom_commands.cpp        custom_npc.cpp            custom_gameobject.cpp
custom_playerscript.cpp    custom_spells.cpp         warboards.cpp
instance_garr.cpp          Lfg_Solo.cpp             custom_script_loader.cpp
FluxRoom/  -> KeyStoneGenerator.cpp   LevelMaster.cpp      LoginAnnouncer.cpp
              PlayedTimeReward.cpp    StartZoneQuestSkipper.cpp  GuardianAngel.cpp
              Zolocraft.cpp (solocraft) CovenantTest.cpp   FluxFunctions.cpp
              FluxPhaseMgr.cpp        OpcodeTester.cpp     fluxurions_script_loader.cpp
```
Notable features: **Keystone (M+) generator, LevelMaster, LoginAnnouncer, PlayedTime rewards,
StartZone quest skipper, Zolocraft (solocraft), warboards, custom commands/npc/spells/gameobjects.**
SQL: mostly inline/hardcoded; see `useful stuff/Example_CustomStuffHotfixes.sql` for the hotfix pattern.

### 2. WoTLK-Legends-of-Azeroth/AzgathCore  (branch 8.3.7 — has paired SQL) ✅
https://github.com/WoTLK-Legends-of-Azeroth/AzgathCore  (branch: `8.3.7`)
Path: `src/server/scripts/Custom/`
```
DoubleXP.cpp              NPCTeleport.cpp           GOMove/GOMoveScripts.cpp (+ Lua addon)
arwent_gift_mount.cpp     arwent_legit_quest_bypass.cpp   heirloom_mount_tempfix.cpp
spp_save_on_levelup.cpp   custom_npcs.cpp           custom_player_script.cpp
```
**Has matching SQL** (rare + valuable):
```
sql/Azgath/world/NPCTeleport/teleporter_install.sql   teleporter_remove.sql
sql/custom/{auth,characters,hotfixes,world}/...   (custom SQL folders, BFA schema)
```
Features: **NPC Teleporter (+install SQL), GOMove (move gameobjects in-game), DoubleXP,
heirloom mount fix, save-on-levelup, gift mount, quest bypass.**

### 3. zTerragor/Legends-of-Azeroth-BFA  ✅
https://github.com/zTerragor/Legends-of-Azeroth-BFA   Path: `src/server/scripts/Custom/`
```
XpWeekend.cpp     mod_solocraft.cpp     world_chat.cpp     custom_npcs.cpp
custom_player_script.cpp     custom_script_loader.cpp
```
Features: **Solocraft, XP Weekend, World Chat.**

### 4. MttAI-dev/MTT-WoW-BfA  ✅
https://github.com/MttAI-dev/MTT-WoW-BfA   Path: `src/server/scripts/Custom/`
```
Zombie_Mini_Game.cpp   world_chat.cpp   custom_npcs.cpp   custom_player_script.cpp
```
SQL: `sql/LatincoreBfa/world/2020_08_10_01_Custom_Boss_Event.sql` (custom boss event)
Features: **Zombie mini-game, World Chat, custom boss event SQL.**

### 5. BfaCore repack (Varjgard lineage) — changelog has many custom DB/script entries ✅
Thread: https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-emulator-servers/wow-emu-general-releases/925718-bfacore-8-3-7-35662-repack.html
emucoach mirror (2025): https://www.emucoach.com/threads/release-bfacore-repack.7344/
Has: custom boost items (789001-789004), SoloCraft custom script module, WarCampaign scripts,
plus the full BFA world/hotfix SQL fix history. (Original gitlab Varjgard repo is gone — use repack.)

---

## B) LEGION 7.3.5 CORES — their `src/server/scripts/Custom/` contents

### 1. AshamaneProject/AshamaneCore  (branch: legion) ✅
https://github.com/AshamaneProject/AshamaneCore   (branches: `legion`, `master`, `ducktape` — NO bfa branch anymore)
Path: `src/server/scripts/Custom/`
```
custom_npcs.cpp     custom_script_loader.cpp
```
(Small native Custom folder — Ashamane's value is its huge NON-custom Legion script tree:
ClassHalls, Artifact scenarios, Suramar, Legion world bosses, full class spell scripts.)

### 2. mengjingxuan/TrinityCore-Legion  (AshamaneCore mirror) ✅
https://github.com/mengjingxuan/TrinityCore-Legion   Path: `src/server/scripts/Custom/`
```
custom_npcs.cpp     custom_player_script.cpp     custom_script_loader.cpp
```

### 3. darki73/Legion  (Legion dungeon scripts WITH split SQL) ✅
https://github.com/darki73/Legion
Not a `Custom/` folder, but genuine Legion C++ + per-boss/instance SQL for:
Maw of Souls, Halls of Valor, Darkheart Thicket, Eye of Azshara, Black Rook Hold,
Neltharion's Lair, Vault of Wardens, Violet Hold. Great porting/reference material.

### 4. Legion repack threads (scripts + locale SQL) ✅
LegionCore repack: https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-emulator-servers/wow-emu-general-releases/893231-legioncore-7-3-5-repack-version-2020_04_25_final-4.html
  → `LegionCore_world_database_locales.sql`, `LegionCore_hotfixes_database_locales.sql`
Single Player Project – Legion: https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-emulator-servers/wow-emu-general-releases/581179-single-player-project-legion.html
  → cooler-SAI/JadeCore scripts (Molten Core, Twilight Highlands, Tol Barad), lasyan3 custom
    patches (custom respawn/attack speed, SpeedGame, NoCastTime, HurtInRealTime), DH spell scripts.

---

## C) THE COMPLETE UNIQUE CUSTOM-SCRIPT INVENTORY (deduped across BFA/Legion cores)
Every distinct custom script found, with which core has it:

| Custom script / feature | Source core(s) |
|---|---|
| custom_npcs / custom_npc.cpp | all (Dekk, Azgath, LoA, MTT, Ashamane, tc-legion) |
| custom_player_script / custom_playerscript.cpp | Azgath, LoA, MTT, tc-legion, Dekk |
| custom_commands.cpp | Dekk |
| custom_gameobject.cpp | Dekk |
| custom_spells.cpp | Dekk |
| custom_script_loader.cpp | all (the registrar) |
| warboards.cpp | Dekk |
| instance_garr.cpp (garrison) | Dekk |
| Lfg_Solo.cpp (solo LFG) | Dekk |
| KeyStoneGenerator.cpp (M+ keystone) | Dekk (FluxRoom) |
| LevelMaster.cpp | Dekk (FluxRoom) |
| LoginAnnouncer.cpp | Dekk (FluxRoom) |
| PlayedTimeReward.cpp | Dekk (FluxRoom) |
| StartZoneQuestSkipper.cpp | Dekk (FluxRoom) |
| GuardianAngel.cpp | Dekk (FluxRoom) |
| Zolocraft.cpp / mod_solocraft.cpp (solocraft) | Dekk, LoA |
| CovenantTest.cpp | Dekk (FluxRoom) |
| OpcodeTester.cpp | Dekk (FluxRoom) |
| NPCTeleport.cpp (+ teleporter_install.sql) | Azgath |
| GOMove (move gameobjects, +Lua addon) | Azgath |
| DoubleXP.cpp / XpWeekend.cpp | Azgath, LoA |
| arwent_gift_mount.cpp | Azgath |
| arwent_legit_quest_bypass.cpp | Azgath |
| heirloom_mount_tempfix.cpp | Azgath |
| spp_save_on_levelup.cpp | Azgath |
| world_chat.cpp | LoA, MTT |
| Zombie_Mini_Game.cpp | MTT |
| Custom_Boss_Event (SQL) | MTT |
| boost items 789001-789004 (SQL) | BfaCore repack |

---

## D) HOW TO GRAB THEM (tree-only clone, then copy the Custom folder)
```
# example: get DekkCore's Custom scripts without downloading the whole repo blobs
git clone --filter=blob:none --no-checkout https://github.com/devovh/Shadowlands_Dekk-Core
cd Shadowlands_Dekk-Core
git sparse-checkout set src/server/scripts/DekkCore/Custom src/server/scripts/Custom
git checkout
# then copy the .cpp you want into YOUR core's src/server/scripts/Custom/
# and register them in your custom_script_loader.cpp + CMake, then recompile.
```
Repeat for: AzgathCore (branch 8.3.7), Legends-of-Azeroth-BFA, MTT-WoW-BfA, AshamaneCore (legion).

---

## E) PORTING NOTES (BFA/Legion are close family, but not identical)
- BFA (8.3.7) and Legion (7.3.5) share most of the modern TC API → these scripts port with MINOR
  edits (far easier than WotLK). Watch for: opcode/packet name changes, ObjectGuid API, and
  ScriptMgr hook signature differences between 7.3.5 and 8.3.7.
- After copying a `.cpp`: add its `AddSC_xxx()` to your `custom_script_loader.cpp` and to the
  scripts `CMakeLists.txt`, then recompile.
- SQL: AzgathCore's `sql/custom/world` + `NPCTeleport` SQL is BFA-schema and the most reusable.
  MTT's `Custom_Boss_Event.sql` is BFA-schema too. Apply to a BACKUP world DB first.

## F) SQL-GENERATION TOOLS (make your own BFA/Legion custom SQL)
| Tool | Purpose | Link |
|---|---|---|
| WowPacketParser | sniff → SQL (spawns/quests/gossip), supports BFA builds | https://github.com/TrinityCore/WowPacketParser |
| AshamaneProject/WowheadParser | Wowhead → SQL | https://github.com/AshamaneProject/WowheadParser |
| AshamaneProject/VisualSAIStudio | SmartAI editor → SQL | https://github.com/AshamaneProject/VisualSAIStudio |
| AshamaneProject/SpellWork | spell viewer (7.3.5+) for spell scripts | https://github.com/AshamaneProject/SpellWork |

## BOTTOM LINE
The realistic universe of **BFA/Legion-native custom scripts** is the set above (~30 distinct
scripts), concentrated in **DekkCore (richest, 21)** and **AzgathCore (has paired SQL)**, plus the
Legion cores. There is no giant BFA module library like AzerothCore's — the BFA scene is small, so
this list is close to "all of it" that exists publicly in `src/server/scripts/Custom/` form.
Everything else (transmog, bots, etc.) only exists for WotLK and would need a full rewrite.


================================================================================
  BATCH 2 — MORE BFA/LEGION CUSTOM SCRIPTS (newer cores checked, 2026-06-14)
================================================================================
Cloned & inspected NEWER cores not in the first pass. BFA/Legion only.

## NEW BFA cores — src/server/scripts/Custom/

### Mareli/AzgathCore (BFA AzgathCore variant — 9 custom + paired SQL) ✅
https://github.com/Mareli/AzgathCore
```
DoubleXP.cpp        mod_solocraft.cpp     world_chat.cpp        custom_npcs.cpp
custom_player_script.cpp   azgath_gift_mount.cpp   azgath_legit_quest_bypass.cpp
heirloom_mount_tempfix.cpp   custom_script_loader.cpp
```
SQL: sql/azgath/world/NPCTeleport/teleporter_install.sql (+remove) ; sql/custom/{auth,characters,hotfixes,world}
Features: **Solocraft, DoubleXP, World Chat, NPC Teleporter (+SQL), gift mount, quest bypass, heirloom fix.**

### phunny-p4nd4/trinity-core-8.3.7 (BFA) ✅
https://github.com/phunny-p4nd4/trinity-core-8.3.7 — Custom/: custom_script_loader.cpp (minimal native)

### Psychostout/Psycho_Core-8.3.7  &  BLocked-by-google/Psycho_Core-8.3.7 (BFA) ✅
Minimal native Custom/ (custom_script_loader.cpp) — same family as your core.

## NEW Legion cores — src/server/scripts/Custom/

### The-Legion-Preservation-Project/TrinityCore (Legion, actively preserved) ✅
https://github.com/The-Legion-Preservation-Project/TrinityCore
Custom/: **custom_lfg.cpp** (custom LFG — NEW, not seen in other cores), custom_script_loader.cpp

### BlaMacfly/ArgusCore (branch: legion) ✅
https://github.com/BlaMacfly/ArgusCore  (branch `legion`) — Custom/: custom_npcs.cpp, custom_script_loader.cpp

### MrDeadNoob/TrinityCore-7.3.5-26972 (Legion) ✅
https://github.com/MrDeadNoob/TrinityCore-7.3.5-26972 — Custom/: custom_script_loader.cpp

### hwis/TrinityCore-Legion  &  phunny-p4nd4/TrinityCoreLegion (Legion) ✅
Minimal native Custom/ (custom_script_loader.cpp).

## ⭐ BIG FIND — BLocked-by-google/Psycho_Core-8.3.7-Modules  (BFA-native MODULE repo) ✅
https://github.com/BLocked-by-google/Psycho_Core-8.3.7-Modules
NOT a src/server/scripts/Custom layout — it's a top-level `modules/` style repo (like yours),
with BFA-native modules that EACH bring their own SQL. This is the closest thing to a real BFA
module library found anywhere:

| Module | C++ files | SQL files | What it does |
|---|---|---|---|
| **mod-psychobot** | 69 | 10 | Full AI player-bot system (per-class AI, AhBot, group/login/population mgrs, gear/talent/spec) — auth+characters+world SQL |
| **mod-ollama** | 6 | 1 | AI/LLM chat for NPCs/bots (Ollama integration) — characters SQL |
| **mod-aoeloot** | 3 | 0 | Area-of-effect looting |
| **mod-autoloot** | 3 | 0 | Auto-loot |

> mod-psychobot here is a BFA build of the bot module you already have in your core's `modules/`.
> mod-aoeloot / mod-autoloot are clean, small, BFA-native QoL modules — easy to add.

## BATCH 2 — NEW unique custom scripts added to the master inventory
| Script / module | Source | Notes |
|---|---|---|
| custom_lfg.cpp | Legion-Preservation-Project | custom LFG (Legion) |
| azgath_gift_mount.cpp / azgath_legit_quest_bypass.cpp | Mareli/AzgathCore | (renamed arwent_* variants) |
| mod-aoeloot (module) | Psycho_Core-8.3.7-Modules | BFA AoE loot |
| mod-autoloot (module) | Psycho_Core-8.3.7-Modules | BFA auto loot |
| mod-ollama (module +SQL) | Psycho_Core-8.3.7-Modules | BFA AI NPC chat |
| mod-psychobot (module +SQL) | Psycho_Core-8.3.7-Modules | BFA AI bots (69 cpp/10 sql) |

## RUNNING TOTAL of distinct BFA/Legion custom scripts & modules found (Batch 1 + 2)
- Custom/ C++ scripts: ~32 distinct (DekkCore richest at 21; AzgathCore variants; LoA; MTT; +custom_lfg)
- BFA-native MODULES (with SQL): mod-psychobot, mod-ollama, mod-aoeloot, mod-autoloot
- Cores checked total: DekkCore, AzgathCore (WoTLK-Legends + Mareli), LoA-BFA, MTT-WoW-BfA,
  AshamaneCore(legion), TC-Legion, darki73/Legion, ArgusCore, Legion-Preservation-Project,
  MrDeadNoob, hwis, phunny-p4nd4 (x2), Psychostout, BLocked-by-google (+Modules).

## CONCLUSION (batch 2)
The newer cores mostly carry the SAME small native Custom/ set, BUT two real additions surfaced:
(1) **custom_lfg.cpp** (Legion Preservation Project), and (2) the **Psycho_Core-8.3.7-Modules** repo
with **mod-aoeloot, mod-autoloot, mod-ollama, mod-psychobot** — genuine BFA modules + SQL.
For your bfa-core, that Modules repo is the highest-value batch-2 find (drop-in module format).


================================================================================
  BATCH 3 — MORE BFA/LEGION CUSTOM SCRIPTS (2026-06-14, deeper core sweep)
================================================================================
Checked forks + more Legion cores. BFA/Legion only. TWO big new sources found.

## ⭐⭐ zitengzela/LegionCore-7.3.5V2  — RICHEST Custom folder found anywhere (26 scripts) ✅
https://github.com/zitengzela/LegionCore-7.3.5V2   Path: src/server/scripts/Custom/
```
custom_arena_1v1.cpp        arena_spectator.cpp        command_arena.cpp        bracket.cpp
multi_vendor.cpp            npc_beastmaster.cpp        npc_profession.cpp       npc_quest_giver.cpp
custum_trainer.cpp          teleguy.cpp                GoMove.cpp               RandomEnchants.cpp
boss_announcer.cpp          announce_login.cpp         who_logged.cpp           traffic_log.cpp
command_donate.cpp         custom_reward.cpp          custom_invasion_event.cpp duel.cpp
darkmoon.cpp               Loskutik.cpp               custom_lf .cpp (solo LFG)
event_april.cpp           event_tarecgosa.cpp        midsummer_fire_festival.cpp
```
Features (Legion 7.3.5): **1v1 Arena, Arena Spectator, Arena brackets, Multi-Vendor, Beastmaster,
Profession NPC, Quest-Giver NPC, custom Trainer, Teleporter (teleguy), GoMove, Random Enchants,
Boss Announcer, Login Announce, Who-Logged, Traffic Log, Donate command, custom Reward, custom
Invasion Event, Duel mods, Darkmoon, Loskutik, Solo LFG, + holiday events (April Fools, Tarecgosa,
Midsummer Fire Festival).**
SQL: self-contained (one example: sql/updates/world/2023_03_09_npc_trainer.sql). Most need only the
.cpp + a creature_template NPC entry to spawn. >>> This is the best single Legion custom haul. <<<

## AzgathCore/AzgathCoreLegion  — 12 custom (adds new ones vs BFA Azgath) ✅
https://github.com/AzgathCore/AzgathCoreLegion   Path: src/server/scripts/Custom/
```
DoubleXP.cpp   mod_solocraft.cpp   spp_lfg_solo.cpp   premium.cpp   duelrest.cpp
azgath_guid_flying.cpp   azgath_gift_mount.cpp   azgath_legit_quest_bypass.cpp
heirloom_mount_tempfix.cpp   spp_save_on_levelup.cpp   custom_npcs.cpp   custom_script_loader.cpp
```
NEW vs earlier batches: **premium.cpp, duelrest.cpp, azgath_guid_flying.cpp, spp_lfg_solo.cpp.**

## Other Legion cores checked (smaller native Custom/)
| Core | Custom scripts | New? | Link |
|---|---|---|---|
| dufernst/LegionCore-7.3.5 | CustomStartups.cpp | ✅ new (custom startup) | https://github.com/dufernst/LegionCore-7.3.5 |
| nbyaya/LegionCore-7.3.5 | dungeonbalance.cpp | ✅ new (dungeon scaling) | https://github.com/nbyaya/LegionCore-7.3.5 |
| catontheway/TrinityLegion | custom_npcs.cpp | (dup) | https://github.com/catontheway/TrinityLegion |
| LegacyProjectArchive/Shadowlands_Dekk-Core | full DekkCore 21 set | (dup of Dekk) | https://github.com/LegacyProjectArchive/Shadowlands_Dekk-Core |

## BATCH 3 — NEW unique custom scripts added to master inventory
| Script | Source | Feature |
|---|---|---|
| custom_arena_1v1.cpp | zitengzela | 1v1 arena |
| arena_spectator.cpp / command_arena.cpp / bracket.cpp | zitengzela | arena spectate/brackets |
| multi_vendor.cpp | zitengzela | multi-vendor NPC |
| npc_beastmaster.cpp / npc_profession.cpp / npc_quest_giver.cpp / custum_trainer.cpp | zitengzela | service NPCs |
| teleguy.cpp | zitengzela | teleporter NPC |
| GoMove.cpp | zitengzela | move gameobjects |
| RandomEnchants.cpp | zitengzela | random enchant on loot |
| boss_announcer.cpp / announce_login.cpp / who_logged.cpp / traffic_log.cpp | zitengzela | server announce/logging |
| command_donate.cpp / custom_reward.cpp | zitengzela | donate/reward |
| custom_invasion_event.cpp | zitengzela | invasion world event |
| duel.cpp / darkmoon.cpp / Loskutik.cpp | zitengzela | misc |
| event_april.cpp / event_tarecgosa.cpp / midsummer_fire_festival.cpp | zitengzela | holiday events |
| premium.cpp | AzgathCoreLegion | premium/VIP system |
| duelrest.cpp | AzgathCoreLegion | duel reset/heal |
| azgath_guid_flying.cpp | AzgathCoreLegion | flying by guid |
| spp_lfg_solo.cpp | AzgathCoreLegion | solo LFG |
| CustomStartups.cpp | dufernst | custom startup actions |
| dungeonbalance.cpp | nbyaya | dungeon difficulty scaling |

## RUNNING TOTAL (Batch 1 + 2 + 3) — distinct BFA/Legion custom scripts/modules
- Custom/ C++ scripts: ~60+ distinct now (zitengzela alone added 26 Legion ones)
- BFA modules (with SQL): mod-psychobot, mod-ollama, mod-aoeloot, mod-autoloot
- Cores inspected: 22+ (DekkCore, AzgathCore x3 [WoTLK-Legends/Mareli/AzgathCoreLegion], LoA-BFA,
  MTT, AshamaneCore, TC-Legion, darki73, ArgusCore, Legion-Preservation, MrDeadNoob, hwis,
  phunny-p4nd4 x2, Psychostout, BLocked-by-google +Modules, dufernst, nbyaya, zitengzela,
  catontheway, LegacyProjectArchive)

## CONCLUSION (batch 3)
**zitengzela/LegionCore-7.3.5V2 is the jackpot for Legion custom scripts** — 26 ready features
(arena, vendors, service NPCs, events, admin tools). **AzgathCoreLegion** adds premium/duel/flying.
For BFA specifically, the custom-script pool is still smaller (DekkCore + Azgath + the Modules repo
remain the core BFA sources); most of the rich custom variety lives on the Legion side and would
need porting 7.3.5 -> 8.3.7 (close family, minor edits). This batch roughly DOUBLED the inventory.


================================================================================
  BATCH 4 — VERIFICATION SWEEP + BEST-MAINTAINED VARIANT (2026-06-14)
================================================================================
Checked 9 more Legion/BFA cores (forks of the jackpots + Reforged variants).
Result: mostly MIRRORS — but found the most COMPLETE + best-maintained variant.

## ⭐ BEST VARIANT: Legion-Pandaria-Preservation-Project/LegionCore-7.3.5V2 (27 custom) ✅
https://github.com/Legion-Pandaria-Preservation-Project/LegionCore-7.3.5V2
= the full zitengzela 26-script set **PLUS a merged-in Solocraft.cpp** (27 total), and it is
ACTIVELY MAINTAINED (many fix branches: Broken Shore scenario, Garrisons, packet fixes, CMake).
>>> For Legion custom scripts, prefer THIS repo over zitengzela's original — same scripts, fixed. <<<

## Other cores checked this batch
| Core | Custom/ | Verdict | Link |
|---|---|---|---|
| Winfidonarleyan/LegionCore-7.3.5V2 | 26 | exact mirror of zitengzela | https://github.com/Winfidonarleyan/LegionCore-7.3.5V2 |
| arthurcik/Lcore-7.3.5V2 | 26 | exact mirror | https://github.com/arthurcik/Lcore-7.3.5V2 |
| k4s1pro/LegionCore-7.3.5V2 | 26 | exact mirror | https://github.com/k4s1pro/LegionCore-7.3.5V2 |
| Psychostout/merged-reforged_LegionCore-7.3.5 | 2 (Solocraft) | Reforged merge | https://github.com/Psychostout/merged-reforged_LegionCore-7.3.5 |
| The-Legion-Preservation-Project/LegionCore-7.3.5 | 2 (Solocraft) | preservation | https://github.com/The-Legion-Preservation-Project/LegionCore-7.3.5 |
| nbyaya/LegionCore-7.3.5II | 2 (CustomStartups, Solocraft) | minor | https://github.com/nbyaya/LegionCore-7.3.5II |
| w4509692984/LegionCore-7.3.5 | 1 (CustomStartups) | minor | https://github.com/w4509692984/LegionCore-7.3.5 |
| Titans-Project/LegionCore-Reforged | 1 (loader only) | base | https://github.com/Titans-Project/LegionCore-Reforged |

## BATCH 4 — new (only one new script type)
| Script | Source | Feature |
|---|---|---|
| Solocraft.cpp (standalone capitalized variant) | LPPP / Psychostout / LPP / nbyaya2 | solocraft (party scaling) |

## CONCLUSION (batch 4)
Diminishing returns confirmed: the Legion custom universe has CONVERGED on the
**zitengzela 26-script set**, which is mirrored across ~6 repos. The single best copy is
**Legion-Pandaria-Preservation-Project/LegionCore-7.3.5V2 (27, actively fixed)**. No new
script TYPES appeared beyond standalone Solocraft.cpp. The BFA-side custom pool is unchanged
(DekkCore + Azgath variants + Psycho_Core-8.3.7-Modules remain the BFA sources).

>>> RECOMMENDED SOURCES (final, after 4 batches) <<<
  LEGION custom scripts (26-27):  Legion-Pandaria-Preservation-Project/LegionCore-7.3.5V2
  BFA custom scripts (21):        devovh/Shadowlands_Dekk-Core  (DekkCore/Custom + FluxRoom)
  BFA custom + paired SQL:        WoTLK-Legends/AzgathCore (8.3.7) + Mareli/AzgathCore + AzgathCoreLegion
  BFA modules (+SQL):             BLocked-by-google/Psycho_Core-8.3.7-Modules (psychobot/ollama/aoeloot/autoloot)
