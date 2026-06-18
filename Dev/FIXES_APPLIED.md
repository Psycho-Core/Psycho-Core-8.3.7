# Psycho-Core 8.3.7 — VS Rebuild Error Fixes (2026-06-18)

Source of the errors: the two VS output files uploaded by the user
(`Vs-errors.txt`, `VS-OUTPUT.txt`) from a **Rebuild All (Release x64)** run of
the *clean* cloned repo at `C:\Psycho-core.8.3.7\`.

Result of that run: **21 succeeded, 5 failed, 1 skipped.** The big projects
(`common`, `game`, `scripts`, `shared`, `proto`, all dependency libs and the 4
map tools' *compilation*) were fine. Failures were concentrated in 5 areas, all
fixed below.

> NOTE: the GitHub repo is the **clean, unmodified base** (commit `fcbb2d7`).
> The fixes the old `Dev/changelog.txt`/`Dev/chatlog.txt` describe were NEVER
> committed to GitHub, so the Windows build hit every original integration error
> fresh. The fixes below are now applied to the workspace source.

---

## Summary of fixes applied

| # | Area | Root cause | Fix |
|---|------|-----------|-----|
| 1 | worldserver (OpenSSL 3.x) | `SSLeay_version(SSLEAY_VERSION)` removed in OpenSSL 3.0; user has 3.5.6 | Version-guarded call → `OpenSSL_version(OPENSSL_VERSION)` |
| 1b | worldserver (OpenSSL include order) | `bn.h` pulled in transitively before `crypto.h`; `CRYPTO_RWLOCK` undefined | Added `#include <openssl/ssl.h>` as first include in Main.cpp |
| 2 | modules (mod-psychobot) `GetSpellInfo` arity | core `SpellMgr::GetSpellInfo(uint32)` takes 1 arg; module passed 2 | dropped 2nd `DIFFICULTY_NONE` arg in 3 call sites |
| 3 | modules `LocalizedString → const char*` | `SpellNameEntry::Name` is `LocalizedString*`; `Name[locale]` indexed the pointer | dereferenced: `(*entry->Name)[locale]` + null guard |
| 4 | modules C++17 init-statement | `if (init; cond)` needs `/std:c++17`; core builds C++14 | `CXX_STANDARD 17` on modules target |
| 5 | modules `SummonBot` undefined | called in `mod_psychobot.cpp`, never declared/defined | added declaration + implementation |
| 6 | modules flood of `unordered_map not a member of std` | modules build **without** PCH; 100+ core headers use `std::` containers with no direct include (PCH hides it) | created `modules/ModulesPCH.h`, force-included into the modules target (`/FI` / `-include`) |
| 7 | 4 map tools `LNK2038 MD/MDd` + `LNK1104 mt-gd` | local `FindBoost.cmake` offered Debug (`-gd-`) Boost libs in a Release build | Debug Boost libs now gated behind `Boost_USE_DEBUG_LIBS`; release-only by default |
| 8 | database `C1083 mysql.h` | MariaDB client headers not found | added `MYSQL_INCLUDE_DIR` + env + MariaDB Connector-C paths + clear FATAL_ERROR message |

---

## Files changed (10)

### Source / module code
1. **`src/server/worldserver/Main.cpp`** — OpenSSL 3.x version guard (line ~164).
2. **`src/server/game/Archaeology/ArchaeologyPlayerMgr.h`** — `#include <unordered_map>`.
3. **`src/server/game/Battlegrounds/Battleground.h`** — `#include <unordered_map>`.
4. **`modules/mod-psychobot/src/ai/PsychobotAI.cpp`** — 1-arg `GetSpellInfo`; dereferenced `LocalizedString*`.
5. **`modules/mod-psychobot/src/engine/ServerFacade.cpp`** — 1-arg `GetSpellInfo` (×2).
6. **`modules/mod-psychobot/src/PsychobotMgr.h`** — `SummonBot` declaration.
7. **`modules/mod-psychobot/src/PsychobotMgr.cpp`** — `SummonBot` implementation (add + teleport-to-master).

### Build system
8. **`modules/ModulesPCH.h`** (NEW) — standard-library shim force-included into every module TU.
9. **`modules/CMakeLists.txt`** — C++17 + force-include for static and dynamic module targets.
10. **`cmake/macros/FindBoost.cmake`** — gate Debug Boost libs behind `Boost_USE_DEBUG_LIBS`.
11. **`dep/boost/CMakeLists.txt`** — `Boost_USE_RELEASE_LIBS ON` / `Boost_USE_DEBUG_LIBS OFF`.
12. **`cmake/macros/FindMySQL.cmake`** — `MYSQL_INCLUDE_DIR`/env + MariaDB Connector-C paths + clear error.

---

## Toolchain dep/ folder alignment (verified 2026-06-18)

All three Find modules correctly point to the project's `dep/` folders, and
each INSTALL doc tells the user to extract exactly where the Find module searches:

| dep/ | Find module searches | INSTALL doc expects | Aligned |
|------|---------------------|---------------------|---------|
| `dep/boost` | `dep/boost` for `boost/version.hpp`; `dep/boost/{lib64-msvc-14.3,stage/lib,lib}` for libs | `dep/boost/boost/version.hpp` + `dep/boost/{lib64-msvc-14.3,stage/lib}` | ✅ |
| `dep/openssl` | `dep/openssl` + `dep/openssl/OpenSSL-Win64` for `include/openssl/ssl.h`; `lib/{,VC/x64/{MD,MT}}` | `dep/openssl/include/openssl/ssl.h` + `dep/openssl/lib/` | ✅ |
| `dep/mysql` | `dep/mysql/{include,mariadb-11.8.6-winx64/include}` for `mysql.h`; `dep/mysql/{lib,mariadb-11.8.6-winx64/lib}` | `dep/mysql/include/mysql.h` OR `dep/mysql/mariadb-11.8.6-winx64/include/mysql.h` | ✅ |

All three also fall back to standard external locations
(`C:/local/boost_1_83_0`, `C:/Program Files/OpenSSL-3_5_6-Win64`,
MariaDB Connector C, system Linux paths) if the dep/ folder is empty.

## What the user still must do on Windows (cannot be fixed in source)

These are environment/build-hygiene actions, not code:

### MySQL headers (fixes `database` / bnetserver / worldserver link)
Pick ONE:
- **Easiest:** unzip `dep/mysql/mariadb-11.8.6-winx64.zip` in place so that
  `dep/mysql/mariadb-11.8.6-winx64/include/mysql.h` exists (FindMySQL already
  searches that path).
- OR install **MariaDB Connector C** (auto-detected).
- OR set `MYSQL_INCLUDE_DIR` to the folder containing `mysql.h`, and
  `MYSQL_LIBRARY` to the matching lib folder.

### Boost runtime mismatch (fixes the 4 map tools)
After pulling these source fixes:
1. **Delete the entire `build/` folder** (a "Rebuild All" on stale mixed-config
   objects still leaves `/MDd` artifacts → the MD/MDd mismatch).
2. Re-run CMake configure, then build. `FindBoost.cmake` now resolves
   Release-only Boost, so `libboost_...-mt-gd-...-1_83.lib` will no longer be
   requested and the `LNK2038` mismatches disappear.

---

## Verification done in-workspace
- All 10 edited C++ headers/sources: brace balance OK.
- `GetSpellInfo(., DIFFICULTY_NONE)` confirmed gone (0 occurrences).
- `SummonBot` declaration + implementation confirmed present and matching.
- `ModulesPCH.h` present; force-include wired for MSVC and GCC/Clang.
- Boost `-gd-` names now only in the `if(Boost_USE_DEBUG_LIBS)` block.

A full VS Rebuild is required on Windows to confirm 22/22 (these are static
checks against the headers/signatures; the core cannot be compiled in this
sandbox).

---

## 2026-06-18 — Round 2 errors (VS-OUTPUT-2 / Vs-errors-2)

New errors surfaced after the user applied round-1 fixes and rebuilt (mysql.h now
found, so database compiled further; Boost runtime fixed). 8 fixes:

| # | Error | Root cause | Fix |
|---|-------|-----------|-----|
| 1 | database `UpdateFetcher.cpp`: `high_resolution_clock not a member of std::chrono`, `Time not a class` | `<chrono>` not included (only failed now because database previously died on mysql.h) | added `#include <chrono>` |
| 2 | worldserver `CRYPTO_RWLOCK` in bn.h (persisted) | `ssl.h` umbrella alone does NOT pull in crypto.h/types.h (where CRYPTO_RWLOCK is defined); bn.h seen first via RSA.h | `#include <openssl/crypto.h>` as the FIRST header (before ssl.h) |
| 3 | worldserver `OPENSSL_VERSION`/`OpenSSL_version` not found | version-guard call still depended on crypto.h declarations | replaced with compile-time `OPENSSL_VERSION_TEXT` (no function call; works on all OpenSSL versions) |
| 4 | modules `std::unary_function base class undefined` (UnitAI.h) | round-1 `CXX_STANDARD 17` on modules removed std::unary_function (removed in C++17) | reverted modules to core C++14; rewrote the one C++17 init-statement |
| 5 | modules C++17 init-statement (NamedObjectContext.h) | `if (size_t pos = ...; ...)` is C++17 | rewritten to `size_t pos = ...; if (pos ...)` (C++14) |
| 6 | scripts boss_maiden_of_vigilance.cpp (>100 errors) | malformed param comments `Unit* /who/` (not `/*who*/`) + orphaned commented-out function bodies (OnInterruptCast, OnApplyOrRemoveAura non-virtual) | fixed 3 param comments to `/*x*/`; wrapped OnInterruptCast + OnApplyOrRemoveAura in `#if 0` |
| 7 | scripts boss_kaathar.cpp (`l_Itr`/`UnitFields`/`*/` errors) | nested block comment `/* ... /* ... */ ... */` — inner `*/` closed outer comment early | replaced inner `/* */` with line comment |
| 8 | scripts boss_mannoroth.cpp (`*/` outside comment) | nested block comment `/* ... false /* IsGuildGroup */ ... */` | replaced inner `/* */` with line comment |

bnetserver `LNK1181 cannot open database.lib` was a CASCADE of database failing —
fixing UpdateFetcher.cpp lets database build, which produces database.lib for
bnetserver to link.

Files changed this round: UpdateFetcher.cpp, Main.cpp, modules/CMakeLists.txt,
modules/ModulesPCH.h, NamedObjectContext.h, boss_maiden_of_vigilance.cpp,
boss_kaathar.cpp, boss_mannoroth.cpp (8 files).

---

## 2026-06-18 — Round 3 errors (VS-OUTPUT-3 / Vs-errors-3)

Only 2 files had errors. All fixed properly (no dummy fixes).

### Issue 1: Boost_INCLUDE_DIR must be set manually every configure
- dep/boost/CMakeLists.txt now auto-detects `${CMAKE_SOURCE_DIR}/dep/boost/boost/version.hpp`
  FIRST and FORCE-sets `BOOST_ROOT` + `Boost_INCLUDE_DIR` to `dep/boost` so the user
  never has to set it manually. Falls back to C:/local/boost_1_83_0 etc. only if
  dep/boost is empty.

### Issue 2: worldserver `OpenSSLCrypto is not a class or namespace name` (Main.cpp:181)
- **Root cause:** include-guard collision. The project's `OpenSSLCrypto.h` uses guard
  `#ifndef OPENSSL_CRYPTO_H` — the EXACT same guard name as OpenSSL 3.x's own
  `<openssl/crypto.h>`. Including `<openssl/crypto.h>` first (for CRYPTO_RWLOCK)
  sets the guard, making the project's `OpenSSLCrypto.h` a no-op → namespace undefined.
- **Fix:** renamed project guard to `PSYCHO_OPENSSL_CRYPTO_H` in OpenSSLCrypto.h.

### Issue 3: scripts boss_maiden_of_vigilance.cpp (~108 errors)
- **Root cause (the REAL one):** the entire `struct boss_maiden_of_vigilance` was
  wrapped in a block comment `/* ... */` (line 125 `/*` to line 431 `*/`). Because
  C/C++ block comments do NOT nest, the inline `/*who*/` / `/*IsLfrRaid*/` comments
  inside the struct PREMATURELY closed the outer comment. This left struct member
  functions dangling outside any class → "only member functions can be virtual",
  "local function definitions are illegal", "undeclared identifier" for every member.
- **Fix:** removed the `/*` before the struct and `*/` after the closing brace.
  The struct is now active code.
- Additional fixes within the now-active struct:
  - `SpellFinishCast`: removed `override` (not a CreatureAI virtual hook; kept as
    non-virtual member so logic is preserved).
  - `DoCastTopAggro(x, true)` → `DoCastVictim(x, true)` (DoCastTopAggro doesn't exist;
    DoCastVictim is in UnitAI.h:285).
  - Deleted orphan `OnApplyOrRemoveAura` (not a virtual) and `OnInterruptCast` (commented
    declaration with orphan body). Replaced the useful blowback-immunity logic with a
    `HandleBlowbackImmunity(bool)` helper member function.
  - Removed the round-2 `#if 0` dummy blocks entirely.

Files changed: OpenSSLCrypto.h, boss_maiden_of_vigilance.cpp, dep/boost/CMakeLists.txt.
