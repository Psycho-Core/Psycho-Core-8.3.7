<!--
===========================================================================
 Copyright (c) 2026 Psycho-core. All rights reserved.
 Original work authored 100% from scratch for Psycho_Core.
 Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 NOT covered by the base GPL framework license. Development/evaluation only.
===========================================================================
-->

# How to install modules (Psycho_Core 8.3.7)

Installing a third-party (or your own) module is a **clone + rebuild** — no core
edits required.

---

## 1. Drop the module into `modules/`

```bash
# from the repository root
cd modules
git clone <module-repo-url> mod-<name>     # the folder MUST contain a src/ dir
cd ..
```

If you downloaded a `.zip`, extract it so the path is `modules/mod-<name>/src/...`.
GitHub archives often add a `-master` suffix — rename the folder to drop it
(e.g. `mod-foo-master` → `mod-foo`).

> ⚠️ **Compatibility:** this is a **BfA 8.3.x** core. Modules written for other
> cores/expansions (AzerothCore WotLK 3.3.5, other TC branches) will **not**
> compile unmodified. Use modules built for this core or port them.

---

## 2. Rebuild

```bash
cmake -S . -B build -DMODULES=static     # or dynamic
cmake --build build -j$(nproc)
cmake --install build                     # if you install to a server dir
```

Confirm your module appears in CMake's "Module configuration" graph.

Per-module control:
```bash
cmake -S . -B build -DMODULE_MOD_<NAME>=disabled   # skip one module
cmake -S . -B build -DMODULE_MOD_<NAME>=dynamic    # build just this one as a .so/.dll
```
(`mod-foo` → `MODULE_MOD_FOO`.)

---

## 3. Configuration

- Each module ships `conf/<name>.conf.dist`. The build copies it to
  `configs/modules/` (next to the server and into the install dir).
- Copy `<name>.conf.dist` → `<name>.conf` and edit. Keys are read by the same
  config manager as `worldserver.conf`.

---

## 4. SQL (databases)

The auto-updater applies SQL from directories registered in each database's
`updates_include` table. Pick one:

**A) Quick (use the pre-registered custom folders):**
```bash
cp modules/mod-<name>/sql/world/*.sql        sql/custom/world/
cp modules/mod-<name>/sql/characters/*.sql   sql/custom/characters/   # if any
```
These are applied automatically on next start (`Updates.EnableDatabases`,
`Updates.AutoSetup = 1` are default-on).

**B) Per-module (register the module's own sql/ once):**
```sql
-- run against the matching database (world / characters / auth / hotfixes)
INSERT IGNORE INTO `updates_include` (`path`, `state`)
VALUES ('$/modules/mod-<name>/sql/world', 'MODULE');
```
`$` expands to your configured `SourceDirectory` at runtime. After that, files
placed in that folder are auto-applied.

> Make sure `SourceDirectory` in `worldserver.conf` points at the repo root so
> the updater can find both the core SQL and module SQL.

---

## 5. Start the server

```bash
./worldserver
```
- **Static** modules are compiled in and always active (respect their config
  `*.Enable` keys).
- **Dynamic** modules load from `bin/scripts/` and can be **hot-reloaded** at
  runtime without restarting the server.

---

## 6. Troubleshooting

| Symptom | Fix |
|---|---|
| Module not detected | Folder must be `modules/mod-<name>/src/...`. Re-run `cmake` (configure). |
| Linker error: `Add<...>Scripts` undefined | The loader fn name must match the folder name with `-`→`_`. |
| Duplicate script name assert | Each `*Script("name")` must be globally unique; prefix with the module name. |
| Config keys ignored | `*.conf` must be in `configs/modules/`; check the `[worldserver]` section header. |
| SQL not applied | Ensure the path is registered in `updates_include` (or use `sql/custom/`), and `SourceDirectory` is correct. |
| Won't compile | It's probably written for a different core/expansion (not BfA 8.3). |
