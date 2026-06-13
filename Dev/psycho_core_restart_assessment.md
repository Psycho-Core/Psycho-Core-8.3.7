# Psycho_Core 8.3.7 Restart Assessment

Date: 2026-06-13

Compared repos:

- `Psycho-Core/Psycho_Core-8.3.7` — your current core
- `Titans-Project/BfaCore-Reforged`
- `dio85/BfaCore-Reforged`
- `Bloodtigress/BfaCore-Reforged`
- `zTerragor/Legends-of-Azeroth-BFA`
- `AshamaneProject/AshamaneCore`
- `WoWEmulationProject/AshamaneCoreBFA`

---

## Executive summary

Your core is clearly **Ashamane-derived / Ashamane-close**, but with:

- updated toolchain,
- `modules/` support,
- `mod-psychobot`,
- socketless bot-session core edits,
- a large SQL tree.

For your stated goal — **most complete BFA 8.3.7-and-below content source** — restarting from a stronger BFA content base is likely less work than pulling all BfaCore/Legends/Ashamane content into your current core.

Recommended direction:

> **New base:** `dio85/BfaCore-Reforged` or `Bloodtigress/BfaCore-Reforged`  
> **Fallback base:** `Titans-Project/BfaCore-Reforged`  
> **Primary DB/content donor:** `zTerragor/Legends-of-Azeroth-BFA`  
> **Secondary DB/reference donor:** AshamaneCore/AshamaneCoreBFA  
> **Port from Psycho:** toolchain, module support, `mod-psychobot`, and the small socketless-session core hook set.

---

## High-level repo counts

| Repo | Scripts files | `src/server/game` files | `src/common` files | Module files | SQL files | AddSC registrations incl modules |
|---|---:|---:|---:|---:|---:|---:|
| Psycho | 832 | 667 | 131 | 163 | 6707 | 645 |
| Titans BfaCore | 1524 | 743 | 133 | 0 | 36 | 1319 |
| Dio85 BfaCore | 1524 | 743 | 133 | 0 | 36 | 1319 |
| Legends BFA | 1497 | 741 | 133 | 0 | 269 | 1303 |
| Ashamane master | 1278 | 711 | 135 | 0 | 13149 | 1111 |
| AshamaneCoreBFA | 1278 | 709 | 133 | 0 | 12873 | 1111 |

---

## Key result: Psycho is missing a lot of BfaCore script coverage

`Psycho` vs `BfaCore`:

- Common AddSC registrations: **639**
- BfaCore-only AddSC registrations: **680**
- Psycho-only AddSC registrations: **6**

This is the most important metric. It means that if you keep Psycho as the base, you need to port hundreds of BfaCore source scripts and their DB bindings.

If you restart from BfaCore, you need to port:

- your module system,
- `mod-psychobot`,
- socketless bot session hooks,
- updated toolchain/build fixes,
- any Psycho-only custom behavior.

That is a much smaller and more controllable migration.

---

## Expansion/script folder comparison

| Folder | Psycho | BfaCore | Ashamane | Legends | BfaCore - Psycho |
|---|---:|---:|---:|---:|---:|
| AlliedRaces | 1 | 2 | 0 | 2 | +1 |
| BrawlersGuild | 1 | 9 | 0 | 9 | +8 |
| BrokenIsles | 23 | 206 | 202 | 206 | +183 |
| Commands | 43 | 47 | 46 | 47 | +4 |
| Custom | 1 | 5 | 3 | 6 | +4 |
| DarkmoonIsland | 0 | 7 | 7 | 7 | +7 |
| Draenor | 12 | 115 | 50 | 115 | +103 |
| EasternKingdoms | 209 | 263 | 261 | 263 | +54 |
| Events | 3 | 12 | 12 | 12 | +9 |
| Kalimdor | 124 | 190 | 180 | 190 | +66 |
| KulTiras | 1 | 51 | 33 | 51 | +50 |
| Nazjatar | 1 | 13 | 0 | 0 | +12 |
| Northrend | 188 | 188 | 188 | 188 | 0 |
| Nyalotha | 0 | 16 | 0 | 5 | +16 |
| OutdoorPvP | 11 | 11 | 11 | 11 | 0 |
| Outland | 117 | 117 | 117 | 117 | 0 |
| Pandaria | 11 | 125 | 79 | 125 | +114 |
| Pet | 7 | 7 | 7 | 7 | 0 |
| Scenarios | 1 | 21 | 4 | 16 | +20 |
| Spells | 18 | 20 | 21 | 20 | +2 |
| World | 16 | 18 | 17 | 18 | +2 |
| Zandalar | 1 | 66 | 25 | 67 | +65 |

### Interpretation

Psycho has good older WotLK/TBC/Classic-ish script folder parity in places like Northrend/Outland/OutdoorPvP, but it is badly behind BfaCore for:

- Broken Isles / Legion,
- Draenor / WoD,
- Pandaria / MoP,
- Kul Tiras,
- Zandalar,
- Nazjatar,
- Nyalotha,
- scenarios,
- Brawlers Guild,
- Darkmoon Island.

Therefore, for content completeness, **BfaCore is a better base**.

---

## File similarity result

Psycho is much closer to Ashamane than to BfaCore.

### Psycho vs Ashamane master

- `src/server/scripts`: 742 common files, 452 identical
- `src/server/game`: 663 common files, 314 identical
- `src/common`: 131 common files, 105 identical
- `sql`: 6706 common files, 6703 identical

### Psycho vs BfaCore

- `src/server/scripts`: 741 common files, 0 identical
- `src/server/game`: 654 common files, 0 identical
- `src/common`: 130 common files, 0 identical
- `sql`: 0 common files

This confirms that Psycho is not already a BfaCore-like base. It is an Ashamane-like base with your modernizations/modules layered on top.

---

## Keep Psycho or restart?

### Keeping Psycho means porting into your current core

You would need to bring in, roughly:

- 680 BfaCore-only script registrations,
- hundreds of files across BFA/Legion/WoD/MoP folders,
- related DB tables/data/script bindings,
- BFA-specific SQL packs from Legends/BfaCore,
- BfaCore fixes to DB2/hotfix/Azerite/BFA systems,
- compatibility fixes against your current Ashamane-like APIs.

This is likely high-work and high-risk.

### Restarting from BfaCore means porting your custom work

You would need to bring over:

- module build system,
- `mod-psychobot`,
- socketless-session core hooks,
- updated toolchain patches,
- maybe selected Psycho SQL/custom data.

This is much smaller.

### Verdict

For your stated goal, **restart from BfaCore/Dio85 and port your custom work forward**.

---

## Best base choice

### Preferred base

Use `dio85/BfaCore-Reforged` or the identical `Bloodtigress/BfaCore-Reforged` snapshot.

Why:

- Same script coverage as Titans BfaCore.
- Contains later changes in:
  - OpenSSL 3.x support,
  - Azerite packet/DB2 structures,
  - BNet login service,
  - RealmList/worldserver/player changes.

Changed from Titans BfaCore included files such as:

- `cmake/macros/FindOpenSSL.cmake`
- `sql/base/1_auth.sql`
- `sql/base/2_characters.sql`
- `src/common/Cryptography/OpenSSLCrypto.*`
- `src/server/bnetserver/REST/LoginRESTService.cpp`
- `src/server/game/DataStores/DB2LoadInfo.h`
- `src/server/game/DataStores/DB2Metadata.h`
- `src/server/game/DataStores/DB2Structure.h`
- `src/server/game/Entities/Item/AzeriteItem/Azerite*.cpp`
- `src/server/game/Server/Packets/AzeritePackets.*`
- `src/server/worldserver/Main.cpp`

### Fallback

If Dio85/Bloodtigress gives compile/runtime issues, use Titans BfaCore and manually apply your newer toolchain/OpenSSL work.

---

## Migration plan: Psycho module support into BfaCore

Your module system is relatively self-contained. Port these pieces first, before `mod-psychobot`.

### 1. Copy module infrastructure

Copy:

```text
modules/CMakeLists.txt
modules/ModulesLoader.cpp.in.cmake
modules/ModulesLoader.h
modules/README.md
cmake/macros/ConfigureModules.cmake
```

Also copy module documentation if wanted:

```text
docs/HOW_TO_BUILD_A_MODULE.md
docs/HOW_TO_INSTALL_MODULES.md
Dev/ModuleAPI_Reference.txt
```

### 2. Port CMake options

From Psycho `cmake/options.cmake`, port the `MODULES` option block:

```cmake
set(MODULES_AVAILABLE_OPTIONS none static dynamic)
set(MODULES "static" CACHE STRING "Build core with modules (modules/ folder)")
set_property(CACHE MODULES PROPERTY STRINGS ${MODULES_AVAILABLE_OPTIONS})
```

Also port any helpers around:

```cmake
IsDynamicLinkingModulesRequired(...)
```

### 3. Port root `CMakeLists.txt` hook

Add before `add_subdirectory(src)`:

```cmake
if (NOT MODULES STREQUAL "none")
  add_subdirectory(modules)
endif()
```

### 4. Port worldserver linking

In `src/server/worldserver/CMakeLists.txt`, add `modules` to `target_link_libraries(worldserver ...)`:

```cmake
target_link_libraries(worldserver
  PRIVATE
    trinity-core-interface
  PUBLIC
    scripts
    modules
    game
    readline)
```

Also port dynamic module dependency handling:

```cmake
if (WORLDSERVER_DYNAMIC_MODULES_DEPENDENCIES)
  add_dependencies(worldserver ${WORLDSERVER_DYNAMIC_MODULES_DEPENDENCIES})
endif()
```

### 5. Port ScriptMgr loader hook

In `src/server/game/Scripting/ScriptMgr.cpp`, add:

```cpp
void AddModulesScripts();
```

Then inside `ScriptMgr::Initialize()`, after static core scripts load:

```cpp
AddModulesScripts();
```

### 6. Build test with empty module tree

Before adding `mod-psychobot`, verify:

```bash
cmake -S . -B build -DMODULES=static
cmake --build build -j
```

Then test:

```bash
cmake -S . -B build -DMODULES=none
cmake --build build -j
```

Only after both work, add `mod-psychobot`.

---

## Migration plan: mod-psychobot into BfaCore

### 1. Copy module

Copy:

```text
modules/mod-psychobot/
addon/PsychobotUI/
```

### 2. Port socketless-session core edits

Your current core has clearly marked Psychobot S28 edits. These are the critical files:

```text
src/server/game/Handlers/CharacterHandler.cpp
src/server/game/Server/WorldSession.cpp
src/server/game/Server/WorldSession.h
src/server/game/World/World.cpp
```

Markers found:

- `[Psychobot S28] Socketless bot login`
- `m_isBot`
- `m_botRemove`
- `IsBot()`
- `SetBot()`
- `SetBotRemove()`
- `IsBotRemoving()`

These are likely the only unavoidable core edits for psychobot’s socketless login.

### 3. Rebuild module after API errors

Expected BfaCore API differences may appear in:

- `WorldSession`
- `Player::LoginPlayer`
- character loading flow
- group/LFG role APIs
- spell/talent APIs
- auction APIs
- config manager path handling

But this is still far less work than porting 680 missing core scripts into Psycho.

### 4. Apply module SQL

Use module SQL after the new base DB is stable:

```text
modules/mod-psychobot/sql/auth/psychobot_rbac.sql
modules/mod-psychobot/sql/characters/psychobot_strategies.sql
modules/mod-psychobot/sql/world/psychobot_names.sql
```

Do not mix this with Ashamane SQL until worldserver boots cleanly.

---

## Migration plan: updated toolchain

Port these from Psycho after module support works:

- root CMake minimum/version changes,
- `cmake/options.cmake`,
- `cmake/macros/FindOpenSSL.cmake` or `cmake/modules/FindOpenSSL.cmake`,
- Boost/OpenSSL/MariaDB compatibility edits,
- compiler warning fixes,
- any C++ standard updates.

Order matters:

1. Make BfaCore compile as-is.
2. Apply only required OpenSSL/Boost fixes.
3. Add module support.
4. Add psychobot.
5. Add broader toolchain cleanup.

Do not modernize everything at once.

---

## Data/content donor plan after restart

Once your new BfaCore+modules+psychobot base compiles and boots:

### Primary donor: Legends-of-Azeroth-BFA

Port SQL packs in controlled groups:

1. Mechagon/Mecandria spawns
2. Tiragarde Sound / War Campaign
3. Allied race spawns
4. Demon Hunter start zone
5. Profession trainers Boralus/Dazar'alor
6. Quest POI sniffs
7. BFA dungeon loot
8. old-zone fixes: Duskwood, Redridge, Westfall, Gilneas, Lost Isles, Worgen, Panda start
9. Timeless Isle / MoP loot
10. Nyalotha data

### Secondary donor: Ashamane SQL

Use only after Legends packs are exhausted or if a specific missing system/data set exists there.

Avoid direct import of:

- auth base,
- characters base,
- full world base,
- full hotfix base.

Use explicit column mapping and staging DBs.

---

## Recommended branch strategy

```bash
git clone https://github.com/dio85/BfaCore-Reforged.git Psycho_Core_Next
cd Psycho_Core_Next
git checkout -b psycho-next/base-bfa

git remote add psycho https://github.com/Psycho-Core/Psycho_Core-8.3.7.git
git remote add legends https://github.com/zTerragor/Legends-of-Azeroth-BFA.git
git remote add ashamane https://github.com/AshamaneProject/AshamaneCore.git
git fetch --all
```

Suggested branch sequence:

```text
psycho-next/base-bfa
psycho-next/toolchain
psycho-next/modules
psycho-next/psychobot-core-hooks
psycho-next/mod-psychobot
psycho-next/legends-sql-pack-01
psycho-next/legends-sql-pack-02
...
```

Commit small and boot-test often.

---

## Final recommendation

Do not continue trying to add Ashamane/BfaCore scripts into current Psycho_Core if the goal is full BFA-and-below content.

Your current core is valuable for:

- toolchain modernization,
- module system,
- psychobot,
- SQL archive/custom work.

But as a content base, it is much lighter than BfaCore/Dio85/Legends.

Best path:

> **Restart from Dio85/Bloodtigress BfaCore. Port Psycho’s toolchain/module system/psychobot forward. Then use Legends and Ashamane as SQL/content donors.**

This should be substantially less work than making the current Psycho tree catch up to BfaCore’s content coverage.
