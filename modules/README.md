<!--
===========================================================================
 Copyright (c) 2026 Psycho-core. All rights reserved.
 Original work authored 100% from scratch for Psycho_Core.
 Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 NOT covered by the base GPL framework license. Development/evaluation only.
===========================================================================
-->

# Psycho_Core 8.3.7 — Modules

This folder holds **modules** — self-contained, drop-in extensions to the core,
in the AzerothCore `mod-<name>` style. The build system auto-discovers any
subfolder here that contains a `src/` directory and compiles it into the server.

> ℹ️ Modules reuse the core's existing script system, so they support **static**
> linkage (compiled into `worldserver`) or **dynamic** linkage (separate
> `.so`/`.dll` that supports **hot reload**).

---

## Installing a module

```bash
# From the repository root:
cd modules
git clone <module-repo-url> mod-<name>      # folder MUST contain src/
cd ..
# Reconfigure + rebuild (see docs/HOW_TO_BUILD_A_MODULE.md)
cmake -S . -B build -DMODULES=static
cmake --build build -j
```

That's it — no core edits. On the next configure, the module appears in the
"Module configuration" graph CMake prints.

> ⚠️ **Compatibility:** modules written for other cores/expansions (e.g.
> AzerothCore WotLK 3.3.5) will **not** compile here unmodified — this is a
> **Battle for Azeroth 8.3.x** core with different APIs. Use modules written for
> this core, or port them. Start from `mod-skeleton`.

---

## Linkage / build options

| Option | Meaning |
|---|---|
| `-DMODULES=static` | (default) all modules compiled into `worldserver`. Single binary, simplest deploy. |
| `-DMODULES=dynamic` | each module → its own shared library in `bin/scripts/`. Enables **hot reload** (`.reload` of modules at runtime). |
| `-DMODULES=none` | skip the `modules/` folder entirely. |
| `-DMODULE_MOD_<NAME>=static\|dynamic\|disabled\|default` | override linkage for a single module (e.g. `-DMODULE_MOD_SKELETON=disabled`). |

The module-name → variable mapping uppercases and replaces `-` with `_`
(e.g. `mod-skeleton` → `MODULE_MOD_SKELETON`).

---

## Anatomy of a module

```
modules/mod-<name>/
├── README.md
├── mod-<name>.cmake          OPTIONAL: extra deps/includes, include()d inline
├── conf/
│   └── mod_<name>.conf.dist  OPTIONAL: config, auto-copied to configs/modules/
├── sql/                      OPTIONAL: world/characters/auth/hotfixes SQL
│   └── world/ ...
└── src/                      REQUIRED: the C++ sources
    ├── mod_<name>.cpp            scripts + void AddSC_mod_<name>()
    └── mod_<name>_loader.cpp     void Add<name_with_underscores>Scripts()
```

Rules the build relies on:
- A module is any `modules/<dir>/` that has a **`src/`** folder.
- Each module must define **`void Add<loader>Scripts()`** where `<loader>` is the
  folder name with `-` replaced by `_` (e.g. `mod-skeleton` → `Addmod_skeletonScripts`).
  The generated loader calls it; `ScriptMgr::Initialize()` then runs all of them.
- Config files `conf/*.conf.dist` are copied next to the server and into the
  install `configs/modules/` directory.

---

## Module SQL

The DB auto-updater applies SQL from directories listed in each database's
`updates_include` table. Two ways:

1. **Quick:** put your `.sql` into the pre-registered `sql/custom/<db>/` folders
   at the repo root — applied automatically.
2. **Per-module:** ship `modules/mod-<name>/sql/<db>/...` and register it once:
   ```sql
   -- run against the matching database (world/characters/auth/hotfixes)
   INSERT IGNORE INTO `updates_include` (`path`, `state`)
   VALUES ('$/modules/mod-<name>/sql/world', 'MODULE');
   ```
   (`$` expands to your configured `SourceDirectory`.) Requires
   `Updates.EnableDatabases` and `Updates.AutoSetup = 1` (both default-on).

---

## More docs
- `docs/HOW_TO_BUILD_A_MODULE.md` — write & build a module from scratch.
- `docs/HOW_TO_INSTALL_MODULES.md` — install third-party modules.
- `Dev/ModuleAPI_Reference.txt` — the important core APIs for module authors.
- `mod-skeleton/` — a complete working template.
