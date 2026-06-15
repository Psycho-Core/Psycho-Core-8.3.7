# Psycho_Core 8.3.7 — Reforged + Modernized

<p align="center">
  <img src="assets/Psychocore.swordofsarg2.png" alt="PsychoCore — BFA 8.3.7" width="420">
</p>

> **IMPORTANT REMINDER: CURRENTLY UNDER DEVELOPMENT BY A COMPLETE NOOB WITH AN INTERNET CONNECTION!**

![Status](https://img.shields.io/badge/status-in--development-yellow)
![CMake](https://img.shields.io/badge/CMake-4.3.2-blue)
![C++](https://img.shields.io/badge/C%2B%2B-14-blue)
![Boost](https://img.shields.io/badge/Boost-1.83-orange)
![OpenSSL](https://img.shields.io/badge/OpenSSL-3.5.x-orange)
![MariaDB](https://img.shields.io/badge/MariaDB-11.8.6-orange)
![Client](https://img.shields.io/badge/BfA-8.3.7%20%2835662%29-purple)

A modernized TrinityCore-based emulator for **World of Warcraft: Battle for Azeroth**,
with a refreshed toolchain (CMake 4.3.2, Boost 1.83, OpenSSL 3.5.x) and MariaDB 11.8.6
as the recommended database.

---

## What this is

A **World of Warcraft: Battle for Azeroth 8.3.7 (build 35662)** private-server emulator.

This is *not* a binary release — you compile it yourself from this repository
against your own MariaDB and feed it the data files (maps, vmaps, mmaps, DB2/DBC)
extracted from a real BfA 8.3.7 WoW client.

This is the new **Psycho_Core** base. External upstream/source-base references are
intentionally left out for now and can be added later.

---

## Highlights of this branch

| Area | Change |
|---|---|
| **CMake** | `cmake_minimum_required(VERSION 4.3.2)` |
| **Boost** | Target floor **1.83** on Linux and Windows. |
| **OpenSSL** | Target **3.5.x**. Windows path supports OpenSSL 3.5.6. |
| **MariaDB** | **11.8.6** tested for configure. Windows: unzip/extract the MariaDB x64 client package into `dep/mysql`. |
| **Modules** | Top-level `modules/` folder support with `MODULES=none/static/dynamic`. |
| **Psychobot** | Complete `mod-psychobot` folder is present and detected by CMake. |

---

## Requirements

| Component | Minimum / Target | Notes |
|---|---|---|
| **CMake** | 4.3.2 | Required by this branch. |
| **C++ standard** | C++14 | Set by the core build system. |
| **GCC** (Linux) | 14.2.0 tested | Older compiler support not revalidated yet. |
| **MSVC** (Windows) | VS 2019/2022 | Windows target. |
| **Boost** | 1.83 | Components: system, filesystem, thread, program_options, iostreams, regex. |
| **OpenSSL** | 3.5.x | Linux configure tested with 3.5.6 extracted locally. Windows target: 3.5.6. |
| **MariaDB** | 11.8.6 tested | MySQL-compatible client libraries required. |
| **zlib/bzip2/readline** | system/vendor | Required by normal configure. |

All other third-party libraries are vendored under `dep/`.

---

## Configure status

Configure has been verified successfully.

```text
CMake configure exit code: 0
```

Verified configure dependencies:

```text
CMake:   4.3.2
Boost:   1.83.0
OpenSSL: 3.5.6
MariaDB: 11.8.6 client
```

CMake detected:

```text
MODULES=static
SCRIPTS=static
mod-psychobot
```

Configure output includes:

```text
--   mod-psychobot: full module (engine + 12 classes + Phase D + socketless login) loaded.
-- Configuring done
-- Generating done
```

---

## Build status

> ✅ **bnetserver builds successfully on Linux** using this Psycho_Core branch.
>   - Target built: `bnetserver`
>   - Result: `[100%] Built target bnetserver`
>   - Toolchain: CMake 4.3.2, GCC 14.2.0, Boost 1.83, OpenSSL 3.5.6, MariaDB 11.8.6 client.
>
> ✅ **modules builds successfully**, including `mod-psychobot`.
>
> ⚠️ **worldserver has not been built.** Do not run a worldserver build unless explicitly approved.
>
> ⚠️ **scripts target is still in progress.** Current blocker is a non-PCH include issue:
>   - File: `src/server/scripts/EasternKingdoms/BastionOfTwilight/bastion_of_twilight.h`
>   - Issue: `Position` is used without the required `Position.h` include.

---

## Quick configure

```bash
BOOST_ROOT=/tmp/boost183-root/usr \
BOOST_LIBRARYDIR=/tmp/boost183-root/usr/lib/x86_64-linux-gnu \
OPENSSL_ROOT_DIR=/tmp/openssl356-root/usr \
LD_LIBRARY_PATH=/tmp/openssl356-root/usr/lib/x86_64-linux-gnu:/tmp/boost183-root/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH \
/tmp/cmake-4.3.2-py/cmake/data/bin/cmake -S . -B configure-test \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DSCRIPTS=static \
  -DMODULES=static \
  -DTOOLS=1 \
  -DWITH_WARNINGS=0 \
  -DWITH_COREDEBUG=0 \
  -DBOOST_ROOT=/tmp/boost183-root/usr \
  -DBOOST_LIBRARYDIR=/tmp/boost183-root/usr/lib/x86_64-linux-gnu \
  -DOPENSSL_ROOT_DIR=/tmp/openssl356-root/usr
```

This is configure/generate only. It does not compile.

---

<p align="center">
  <img src="assets/Psychocore.swordofsarg2.png" alt="PsychoCore — BFA 8.3.7" width="420">
</p>

## Repository layout

```
Psycho_Core-8.3.7/
├── CMakeLists.txt
├── README.md
├── cmake/
├── dep/
├── contrib/
├── docs/
├── sql/
├── modules/
│   └── mod-psychobot/
├── src/
│   ├── common/
│   ├── server/
│   │   ├── bnetserver/
│   │   ├── worldserver/
│   │   ├── database/
│   │   ├── proto/
│   │   ├── shared/
│   │   ├── game/
│   │   └── scripts/
│   └── tools/
└── revision_data.h.in.cmake
```

---

## Modules

Psycho_Core supports **drop-in modules** via a top-level [`modules/`](modules/) folder.
Modules support **static** linkage or **dynamic** linkage.

```bash
cmake -S . -B configure-test -DMODULES=static
```

- Module docs: [`modules/README.md`](modules/README.md)
- Build a module: [`docs/HOW_TO_BUILD_A_MODULE.md`](docs/HOW_TO_BUILD_A_MODULE.md)
- Install modules: [`docs/HOW_TO_INSTALL_MODULES.md`](docs/HOW_TO_INSTALL_MODULES.md)

> ⚠️ This is a **BfA 8.3.7** core — modules written for other cores/expansions
> will not compile unmodified.

---

## mod-psychobot

`mod-psychobot` is present in:

```text
modules/mod-psychobot
```

The module folder has been verified complete:

```text
modules/:               163 files
modules/mod-psychobot/: 159 files
```

The module is detected by CMake in static module mode.

Module SQL is shipped under:

```text
modules/mod-psychobot/sql/auth/
modules/mod-psychobot/sql/characters/
modules/mod-psychobot/sql/world/
```

---

## Database status

The public `sql/` folder does not include a complete populated world/hotfix DB package.

Present:

```text
sql/base/1_auth.sql
sql/base/2_characters.sql
sql/updates/world/*
sql/updates/hotfixes/*
sql/updates/characters/*
```

Do not assume the public `sql/` folder alone is enough to run a populated BFA world.

---

## License

This repository is a mixed-license project. Check individual source headers and license files for applicable terms.
