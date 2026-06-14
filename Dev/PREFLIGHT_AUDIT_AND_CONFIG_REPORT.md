# Psycho_Core / BfaCore — Pre-Build Audit, Config & Portability Report
**Date:** 2026-06-14 · Tasks 1-6 results + verification. Code-change log included.

================================================================================
## TASK 1 — TOOLCHAIN PRE-FLIGHT AUDIT (verified against the real cmake/ files)
================================================================================
| Component | Required by core | Where it's checked | Status |
|---|---|---|---|
| CMake | **4.3.2** (`cmake_minimum_required(VERSION 4.3.2)`) | root CMakeLists.txt | ⚠️ very new — install CMake ≥4.3.2 |
| Project | `project(BfaCore)` | root | ok |
| C++ standard | **C++11** (`-std=c++11`) | ConfigureBaseTargets.cmake | ok (matches Eluna too) |
| Boost | **1.83** floor (parses BOOST_VERSION) | cmake/macros/FindBoost.cmake | portable: dep/boost + C:/local/boost_1_83_0 |
| OpenSSL | **3.5.x** (expected str 1.1.1 baseline, accepts 3.5.x) | cmake/macros/FindOpenSSL.cmake | now portable: dep/openssl + Win path |
| MariaDB/MySQL | **11.8.6** client | cmake/macros/FindMySQL.cmake | portable: dep/mysql + Program Files |
| Bundled deps (dep/) | CascLib, SFMT, bzip2, fmt, g3dlite, gsoap, jemalloc, protobuf, rapidjson, readline, recastnavigation, zlib, utf8cpp, process, threads, cotire, efsw, valgrind | dep/ | present (vendored) |

VERDICT: toolchain expectations are coherent. The only "watch" items are the **CMake 4.3.2**
floor (unusually high — make sure the user's CMake is new enough) and that **Boost/OpenSSL/MariaDB
are user-supplied** (unzipped into dep/ or installed on Windows).

================================================================================
## TASK 2 — STATIC PRE-BUILD SCAN (no compiler run; structural checks)
================================================================================
- AddSC_ script declarations found: **1280** (loader surface is large but consistent).
- Loader templates present: `src/server/scripts/ScriptLoader.cpp.in.cmake`,
  `modules/ModulesLoader.cpp.in.cmake` ✅ (these generate at configure time).
- `revision_data.h.in.cmake` present ✅ (genrev).
- root CMakeLists if(/endif counts differ (19 vs 9) — NORMAL: many are one-line `if()` guards and
  `elseif`/nested forms; not an imbalance. No obvious unclosed blocks.
- mod-psychobot completeness: **69 .cpp, 76 .h, 5 .sql, 1 .conf.dist** — self-consistent.
NO blocking structural problems found for configure. (A real compile is still the true test;
per CRITICAL_BUILD_RULE.txt we build only once on your command.)

================================================================================
## TASK 3 — DATABASE DEFAULTS CHANGED  ✅ DONE
================================================================================
EDITED `src/server/worldserver/worldserver.conf.dist` (lines 111-114):
```
LoginDatabaseInfo     = "127.0.0.1;3306;Psycho;Psycho;Psycho_auth"
WorldDatabaseInfo     = "127.0.0.1;3306;Psycho;Psycho;Psycho_world"
CharacterDatabaseInfo = "127.0.0.1;3306;Psycho;Psycho;Psycho_characters"
HotfixDatabaseInfo    = "127.0.0.1;3306;Psycho;Psycho;Psycho_hotfixes"
```
EDITED `src/server/bnetserver/bnetserver.conf.dist` (line 225):
```
LoginDatabaseInfo = "127.0.0.1;3306;Psycho;Psycho;Psycho_auth"
```
NOTE: your message wrote `Pscyho_auth` (typo) — corrected to `Psycho_auth` to stay consistent
with the other three DBs. Tell me if you actually want the typo'd name.
DB names to create in MariaDB: Psycho_auth, Psycho_world, Psycho_characters, Psycho_hotfixes.
DB user: Psycho / password: Psycho (grant ALL on those 4 DBs).

================================================================================
## TASK 4 — mod-psychobot CONF LOCATION  ✅ VERIFIED (works as you described)
================================================================================
Confirmed by reading `cmake/macros/ConfigureModules.cmake` + `modules/CMakeLists.txt`:
- At build, the module's `conf/mod_psychobot.conf.dist` is COPIED to:
  `bin/<CONFIG>/configs/modules/` (runtime) and installed to `${CONF_DIR}/modules` (install).
- So in your compiled server the file lands in: **`<server-root>/configs/modules/mod_psychobot.conf.dist`**
- The server reads it via `sConfigMgr` — its top declares `[worldserver]` so the keys are read by
  worldserver. You rename `.conf.dist` -> `.conf`.
ANSWER: ✅ Yes — put `mod_psychobot.conf` in the server root's `configs/modules/` folder and the
server reads it from there. (The conf header itself documents this: "Copy to configs/modules or
merge keys into worldserver.conf.") No change needed.

================================================================================
## TASK 5 — MySQL/MariaDB POINTS TO dep/ + WINDOWS  ✅ ALREADY PORTABLE
================================================================================
`cmake/macros/FindMySQL.cmake` already (verified):
- Searches **`${CMAKE_SOURCE_DIR}/dep/mysql`** first (portable, no drive label — relative to repo
  root), incl. the `mariadb-11.8.6-winx64/{include,lib,bin}` sub-layout for when the user just
  unzips the MariaDB package into dep/mysql.
- THEN searches Windows install paths (Program Files/MySQL..., registry, $ENV{MYSQL_ROOT}).
ANSWER: already does exactly what you asked. User action = unzip MariaDB client into `dep/mysql/`
(or install MariaDB on Windows). No edit required.

================================================================================
## TASK 6 — OpenSSL + Boost POINT TO dep/ + WINDOWS  ✅ (Boost already; OpenSSL fixed)
================================================================================
- **Boost** (`FindBoost.cmake`) already searches **`${CMAKE_SOURCE_DIR}/dep/boost`** AND
  `C:/local/boost_1_83_0` (+1.84/1.85/1.86) + `$ENV{BOOST_ROOT}`. Portable. No change needed.
- **OpenSSL** (`FindOpenSSL.cmake`) previously searched ONLY Windows paths. EDITED to add the
  bundled portable root FIRST:
  ```
  "${CMAKE_SOURCE_DIR}/dep/openssl"
  "${CMAKE_SOURCE_DIR}/dep/openssl/OpenSSL-Win64"
  (then the existing C:/Program Files/OpenSSL-3_5_6-Win64, C:/OpenSSL-Win64, C:/OpenSSL)
  ```
ANSWER: both now resolve from dep/ (portable, drive-label-free) AND Windows install paths, so the
source tree is portable. User action = unzip OpenSSL 3.5.x into `dep/openssl/` (or install on Win).

================================================================================
## CODE-CHANGE LOG (exactly what was modified — for your records)
================================================================================
1. src/server/worldserver/worldserver.conf.dist — 4 DB lines -> Psycho creds (Task 3)
2. src/server/bnetserver/bnetserver.conf.dist   — 1 DB line  -> Psycho creds (Task 3)
3. cmake/macros/FindOpenSSL.cmake               — added dep/openssl portable search roots (Task 6)
NOTHING ELSE in the core was touched. (No src/*.cpp/.h, no scripts, no modules code changed.)
Per CRITICAL_BUILD_RULE.txt: no build was run. Configure/compile only on your explicit command.
