# Psycho-Core 8.3.7.35662 — WINDOWS Build Guide (step-by-step)
**Target:** Windows x64 · VS 2019/2022 · CMake 4.3.2 · Boost 1.83 · OpenSSL 3.5.x · MariaDB 11.8.6
**Build:** 8.3.7.35662 · Paths shown relative to the repo root where possible.

> Per CRITICAL_BUILD_RULE.txt: build once; if it errors, fix + report + WAIT for "TRY AGAIN".

## 0. Prerequisites (install once)
- Visual Studio 2019 or 2022 with **"Desktop development with C++"** workload.
- **CMake ≥ 4.3.2** (cmake.org) — add to PATH.
- **Git** (for cloning + revision data).
- **MariaDB 11.8.6** (x64) — either install the server, or just grab the client package.
- **Boost 1.83** — easiest: use the pre-compiled packs on the repo's **Releases** tab
  (tag **V1.0-Boost**, "Pre-compiled Boost + EXE"). vc143/x64/1.83. See step 2.
- **OpenSSL 3.5.x** Win64 (slproweb.com offers 3.5.6 — the P-02 patch accepts 3.5.x).

## 1. Get the source
```
git clone https://github.com/Psycho-Core/Psycho-Core-8.3.7
```
Clone anywhere — paths are portable.

## 2. Provide the dependencies (portable way)
Drop/extract into the repo's dep/ folders so the source tree stays portable:
- MariaDB client  -> `dep/mysql/`  (so you get dep/mysql/include, /lib, /bin
  OR dep/mysql/mariadb-11.8.6-winx64/{include,lib,bin})
- OpenSSL 3.5.x   -> `dep/openssl/` (dep/openssl/include, /lib  OR dep/openssl/OpenSSL-Win64/)
- Boost 1.83      -> `dep/boost/`  (see Boost sub-steps below)
(If you prefer system installs in C:\Program Files, the Find macros also detect those.)

### 2a. Boost — extract the pack (REQUIRED before CMake; it can't read a zip)
- **Normal/Release build (recommended):** in `dep/boost\`, right-click
  **`boost_dep_release.zip`** (~26 MB, shipped in the repo) -> **Extract Here**.
  You must end up with `dep/boost/boost/version.hpp` and `dep/boost/stage/lib/`.
- **Debug build:** download **`boost_dep.zip`** (~280 MB) from the **Releases** tab
  (tag **V1.0-Boost**) and extract it into `dep/boost\` instead -> you get
  `dep/boost/boost/` + `dep/boost/lib64-msvc-14.3/`.
- **Trouble / corrupt zip:** download **`boost_1_83_0-msvc-14.3-64.exe`** from the same
  Releases tab, run it, then copy its `boost\` + `lib64-msvc-14.3\` into `dep/boost\`
  (or set env `BOOST_ROOT`). Or build from source — see `dep/boost/INSTALL_BOOST.txt`.
- **Nesting check:** `dep/boost/boost/version.hpp` must exist at THAT path (not
  `dep/boost/boost_1_83_0/boost/...` — if so, move the inner folders up one level).
- Full detail: **`dep/boost/INSTALL_BOOST.txt`**.

## 3. Configure with CMake (out-of-source build dir INSIDE the repo)
```
cd Psycho-Core-8.3.7
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DTOOLS=1 -DSCRIPTS=static -DMODULES=static
```
- `-G "Visual Studio 16 2019"` if on VS2019.
- `-DTOOLS=1` builds the extractors (mapextractor, vmap4extractor/assembler, mmaps_generator).
- `-DMODULES=static` builds mod-psychobot in (use `dynamic` for .dll modules, `none` to skip).
- If Boost/OpenSSL/MySQL aren't found, pass hints, e.g.
  `-DBOOST_ROOT=../dep/boost -DOPENSSL_ROOT_DIR=../dep/openssl`
  (the macros also auto-search dep/ — these are only fallbacks).

## 4. Build
Open the generated `.sln` in `build/` in Visual Studio (note: the upstream CMake still names the
solution `BfaCore.sln` unless the project name is changed in the build system) → set configuration
**RelWithDebInfo** (or Release) →
Build Solution. (Or from cmd: `cmake --build . --config RelWithDebInfo`.)
> First build can take 30-90 min. BUILD ONCE — if it fails, stop, report the error, fix, wait.

## 5. Collect the runtime (portable server root)
After build, binaries land in `build/bin/<Config>/`. Create your SERVER ROOT and copy:
- worldserver.exe, bnetserver.exe (+ any required .dll: libmariadb, libcrypto/libssl, etc.)
- configs/  (worldserver.conf.dist, bnetserver.conf.dist, configs/modules/mod_psychobot.conf.dist)
Rename each `*.conf.dist` -> `*.conf`.

## 6. Databases (MariaDB)
- Create DBs: `Psycho_auth`, `Psycho_world`, `Psycho_characters`, `Psycho_hotfixes`.
- Create user `Psycho` / password `Psycho`; GRANT ALL on those 4 DBs.
- Import base SQL from `sql/base/` then let the worldserver auto-updater apply `sql/updates/`.
- (Defaults in the .conf already point at Psycho/Psycho/Psycho_*.)

## 7. Client data (DataDir)
Put extracted `dbc maps vmaps mmaps cameras gt` into `<server root>/data/`, and set
`DataDir = "./data"` in worldserver.conf. (See PORTABLE_SERVER_SETUP_GUIDE.md + the data-size note:
a complete set is ~10-15 GB.)

## 8. Run
Start `bnetserver.exe` (or authserver) then `worldserver.exe` from the server root. Create a GM
account in the worldserver console: `account create <name> <pass>` then
`account set gmlevel <name> 3 -1`.

## Common Windows gotchas
- "Could not find MySQL/OpenSSL/Boost" -> dep/ folder not populated or wrong sub-layout; check the
  Find*.cmake search paths (they print what they looked for).
- Missing runtime .dll at launch -> copy libmariadb.dll + OpenSSL DLLs next to worldserver.exe.
- CMake too old -> must be ≥ 4.3.2.
