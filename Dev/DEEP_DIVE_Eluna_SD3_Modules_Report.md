# Deep-Dive Research Report — Eluna, ScriptDev3, & Modules-Folder Move
**Date:** 2026-06-14
**Core:** Psycho-Core 8.3.7.35662 (TrinityCore-derived)
**Covers:** (1) Eluna deep dive, (2) Eluna downloaded to Dev/, (3) SD3 worth-it analysis,
(4) moving `modules/` into `src/server/`. For future reference.

================================================================================
## PART 1 — ELUNA DEEP DIVE (everything, verified by cloning the repos)
================================================================================

### 1.1 The branch landscape (verified live)
Eluna repo (github.com/ElunaLuaEngine/Eluna) has exactly THREE branches:
- **`master`** — the actively-developed engine (Lua 5.2 / LuaJIT, modern method set)
- **`tc-retail`** — variant tracking modern retail/master TrinityCore  ← DOWNLOADED for you
- **`cmangos-spell-update`** — cMaNGOS-specific

Official ready-made core+Eluna forks that EXIST:
- ✅ `ElunaTrinityWotlk` (3.3.5) — the ONLY maintained TrinityCore+Eluna merge
- ✅ MaNGOS/cMaNGOS Vanilla/TBC/WotLK forks
- ❌ `ElunaTrinityCata` / `Master` / `Bfa` / `Retail` → **ALL 404. No BFA fork exists.**

> CONCLUSION: There is NO drop-in Eluna for BFA 8.3.7. You MUST hand-merge + backport.

### 1.2 Real Eluna structure (tc-retail, what you now have in Dev/Eluna-tc-retail/)
Files live at REPO ROOT (no `src/` folder):
```
LuaEngine.cpp/.h  ElunaMgr.*  ElunaLoader.*  ElunaConfig.*  ElunaCompat.*
ElunaTemplate.*  ElunaUtility.*  ElunaEventMgr.*  ElunaSpellWrapper.*
ElunaInstanceAI.*  ElunaCreatureAI.h  BindingMap.h  LuaValue.*  lmarshal.*
hooks/    -> 13 hook files: Player/Creature/Guild/GameObject/Gossip/Group/Item/
             Instance/Packet/Server/Spell/Vehicle/BattleGround + Hooks.h + HookHelpers.h
methods/  -> TrinityCore/ (23 .h: Unit, Player, Global, Spell, Item, Map, Quest...),
             plus CMangos/ Mangos/ VMangos/ AzerothCore/ Custom/
modules/  extensions/  docs/(INSTALL,MERGING,USAGE,IMPL_DETAILS)  CMakeLists.txt
```

### 1.3 Requirements (verified)
- **C++11** (Eluna INSTALL.md: "Eluna uses C++11"). YOUR CORE = C++11 too
  (cmake/macros/ConfigureBaseTargets.cmake: `-std=c++11`). ✅ MATCH — no standard bump needed.
- **Lua 5.2** (or LuaJIT) dependency must be present/linked in the core.
- ACE or BOOST for filesystem (your core has Boost). ✅

### 1.4 What porting actually requires (the real work, in order)
1. **Add Lua 5.2 dependency** to `dep/` and link it (the standalone repo doesn't include the
   core-side lua wiring; the WotLK fork does).
2. **Place engine** — mirror ElunaTrinityWotlk: Eluna under the GAME library (e.g.
   `src/server/game/LuaEngine/`), added to that lib's CMake (not just a scripts subdir, because
   Eluna calls deep into Player/Unit/Map/etc.).
3. **ScriptMgr integration** — add a global `sEluna`, init/teardown in `ScriptMgr.cpp`, and drive
   the engine. (Your core has NO `game/Scripting/ScriptLoader.cpp`; the loader is the generated
   `scripts/ScriptLoader.cpp.in.cmake`.)
4. **World::Update(diff)** — call Eluna's update each tick (verify exact name in current
   `LuaEngine.h`). Skipping it = Lua GC never cycles = memory growth/crash.
5. **ObjectGuid mapping** — reconcile Eluna GUID calls vs your 8.3.7
   `Entities/Object/ObjectGuid.h` accessors (GetEntry/GetCounter/etc.).
6. **Hooks** — reconcile all 13 `hooks/*.cpp` signatures against your `ScriptMgr.h` (BFA uses
   explicit multi-arg signatures; retail uses struct payloads). BIGGEST task.
7. **Methods** — fix `methods/TrinityCore/*.h` API mismatches as compiler errors surface
   (AddAura, gossip menu, async DB query map to your `DatabaseEnv.h`).
8. **Deploy** — `lua_scripts/` next to worldserver + Eluna config block.

### 1.5 The smartest strategy (de-risks ~70% of the work)
Clone **ElunaTrinityWotlk** and `git diff` it against vanilla TrinityCore 3.3.5. That diff is the
EXACT list of core files Eluna touches (ScriptMgr, World, ObjectMgr, CMake, config, Map, Player).
Apply the analogous edits to your 8.3.7 core — turns "guess what to change" into a checklist.

### 1.6 Difficulty: 🔴 HARD but POSSIBLE. Days→weeks. Needs real C++ ability (read/fix build errors).
   No "impossible" wall anywhere — it's all bounded, known work.

================================================================================
## PART 2 — ELUNA DOWNLOADED  ✅ DONE
================================================================================
Downloaded the recommended branch (`tc-retail`) into:
  **`Dev/Eluna-tc-retail/`**  (204 files, plain — .git removed, 13 hooks, 23 TC methods)
This is your working copy to study/adapt. Keep ElunaTrinityWotlk handy as the reference diff.

================================================================================
## PART 3 — SCRIPTDEV3 (SD3): IS IT WORTH ADDING?  →  ❌ NO. NOT COMPATIBLE.
================================================================================
Repo: github.com/mangos/ScriptDev3 (commit 10e471f...)

### What SD3 actually is (verified from its README + code)
- README: "**New Script engine for all MaNGOS cores.** Developed from the old ScriptDev2.
  The same code library is used unchanged as a submodule between each of the cores."
- CMake: targets **MaNGOS**, clients **1.12.x, 2.4.3, 3.3.5a, 4.3.4a, 5.4.8** (max = MoP 5.4.8).
- Uses **MaNGOS-only API headers**: `DBCStores.h`, `InstanceData.h`, `GossipDef.h`,
  `system/ScriptDevMgr.h`, `precompiled.h`, `sc_creature.h`, `sc_instance.h`.
- Library name `mangosscript`. ZERO TrinityCore references (one "trinity" hit was just a boss
  filename: boss_alizabal.cpp).
- Content scope: scripts/ for battlegrounds/eastern_kingdoms/kalimdor/maelstrom/northrend/
  outland/world — i.e. CLASSIC→MoP era content, in MaNGOS format.

### Why it does NOT fit your core
| Factor | SD3 | Your core |
|---|---|---|
| Engine family | **MaNGOS** | **TrinityCore** |
| Max client | 5.4.8 (MoP) | 8.3.7 (BFA) |
| Script API | MaNGOS ScriptDevMgr, sc_creature, DBCStores | TrinityCore ScriptMgr, SmartAI, DB2Stores |
| AI base classes | escort_ai/follower_ai/guard_ai (MaNGOS) | TrinityCore ScriptedAI/SmartAI |
| Data stores | DBC (pre-WoD) | DB2 (WoD+) |

SD3 and TrinityCore have **fundamentally different script-registration systems and object APIs**.
Dropping SD3 into your core would not compile — every script would need a full rewrite to the
TrinityCore API. And even fully rewritten, the content is Classic→MoP (which your core already
has via its own TrinityCore-format scripts).

### Verdict on SD3
- **Not worth it.** Wrong engine (MaNGOS), wrong era (≤MoP), incompatible API.
- You'd gain nothing your TrinityCore-based core doesn't already cover, at the cost of a total
  per-script rewrite. **Skip it.**
- (If you ever wanted MaNGOS-style escort/follower AI patterns, TrinityCore already has
  equivalent ScriptedAI/SmartAI — no need for SD3.)

================================================================================
## PART 4 — MOVING modules/ INTO src/server/  —  HOW HARD? WORTH IT?
================================================================================

### How modules/ is wired now (verified in your root CMakeLists.txt)
```
line 91:  add_subdirectory(dep)
line 97:  add_subdirectory(modules)   <-- top-level, BEFORE src, gated by: if(NOT MODULES STREQUAL "none")
line 104: target_compile_options(modules PRIVATE -Wno-narrowing)  (GCC/Clang narrowing relax)
line 109: add_subdirectory(src)
```
- `modules/CMakeLists.txt` is a self-contained build engine ("Mirrors src/server/scripts/
  CMakeLists.txt"), with `ModulesLoader.cpp.in.cmake` + `ModulesLoader.h`, supporting
  static/dynamic/none linkage.
- It is **intentionally added BEFORE `src/`** so the `modules` target + dynamic-module deps exist
  when worldserver / the static loader reference them.

### Difficulty of moving it to src/server/modules/
⭐⭐ **Low–Medium.** It's mostly path edits, BUT there are real ordering/path traps:

To move it you would need to:
1. `git mv modules src/server/modules` (move the folder).
2. In **root CMakeLists.txt**: change `add_subdirectory(modules)` →
   `add_subdirectory(src/server/modules)` — AND keep it BEFORE `add_subdirectory(src)`, OR move
   the include into `src/server/CMakeLists.txt` in the correct order. ⚠️ If the `modules` target
   stops existing before `src` is processed, the `target_compile_options(modules ...)` at line 104
   and any worldserver/static-loader references will FAIL to configure.
3. Fix every **relative path** inside `modules/CMakeLists.txt`, `ModulesLoader.cpp.in.cmake`, and
   the module `.cmake` files that assumes the top-level location (install offsets, source globs,
   the "mirrors src/server/scripts" path math).
4. Re-point the **GetModuleSourceList / GetInstallOffset** macros if they compute paths from the
   project root.
5. Verify `mod-psychobot`'s own `mod-psychobot.cmake` + conf/sql relative paths still resolve.
6. Reconfigure CMake from scratch (delete build dir) and re-test static AND dynamic linkage.

### Will it break anything?
- **It CAN break the build** if the target-ordering (modules before src) or relative paths aren't
  fixed exactly — the line-104 narrowing relax and loader references are the fragile points.
- It will NOT break gameplay/runtime IF configured correctly — the resulting binary is identical;
  this is purely a source-tree organization change.
- Your existing `Dev/API_DIFFERENCE_REPORT.md` shows the module system is already delicately wired
  into WorldSession/ScriptMgr — moving it adds risk for no functional gain.

### Is there much point?
**Honestly, no — low/no benefit, real risk.** Reasons:
- TrinityCore's own convention historically keeps optional module systems at a clear location;
  a top-level `modules/` (like AzerothCore) is a perfectly standard, clean layout.
- Moving it under `src/server/` gives you **zero runtime or functional improvement** — same binary.
- The only upside is cosmetic ("everything under src/"). The downside is breaking a working,
  carefully-ordered CMake graph that already detects `mod-psychobot` and handles static/dynamic.
- Your core's design comment explicitly notes modules are "Configured before src so the modules
  target exists when worldserver / the static loader reference them" — i.e. the current location
  is a deliberate dependency-ordering choice, not an accident.

### RECOMMENDATION (Part 4)
**Leave `modules/` at the top level.** It works, it's standard, and moving it risks the build for a
purely cosmetic gain. If you ever DO move it, do it on a throwaway branch, keep it before `src` in
CMake, fix all relative paths, and full-reconfigure + test both linkage modes before trusting it.

================================================================================
## OVERALL SUMMARY
================================================================================
1. **Eluna**: Possible but a real hand-backport (no BFA fork exists). C++11 (matches your core).
   Use ElunaTrinityWotlk's diff as the change-list. Downloaded tc-retail to Dev/. 🔴 Hard, doable.
2. **Eluna download**: ✅ done → Dev/Eluna-tc-retail/.
3. **SD3**: ❌ Not worth it. MaNGOS engine, ≤MoP, incompatible API — would need total rewrite for
   zero content gain. Skip.
4. **Move modules/ into src/server/**: ⭐⭐ low-medium difficulty, CAN break the CMake build if
   target-ordering/paths aren't fixed, and gives NO functional benefit. **Recommendation: don't.**
