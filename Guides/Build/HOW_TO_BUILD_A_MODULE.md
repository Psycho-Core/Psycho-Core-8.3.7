<!--
===========================================================================
 Copyright (c) 2026 Psycho-core. All rights reserved.
 Original work authored 100% from scratch for Psycho_Core.
 Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 NOT covered by the base GPL framework license. Development/evaluation only.
===========================================================================
-->

# How to build a module (Psycho_Core 8.3.7)

This guide shows how to create, build, and run a module from scratch. The fastest
path is to copy `modules/mod-skeleton` and edit it — but here is the full picture.

---

## 1. The contract

The build auto-discovers any `modules/<dir>/` that contains a **`src/`** folder.
Your module must provide **one** loader function:

```cpp
void Add<loader>Scripts();   // <loader> = folder name with '-' -> '_'
```

For a folder `mod-foo`, that is `Addmod_fooScripts()`. The CMake-generated
`ModulesLoader.cpp` forward-declares and calls it; `ScriptMgr::Initialize()`
invokes the generated `AddModulesScripts()` after the core scripts load.

---

## 2. Minimal module

```
modules/mod-foo/
└── src/
    ├── mod_foo.cpp
    └── mod_foo_loader.cpp
```

`src/mod_foo.cpp`
```cpp
#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"

class mod_foo_PlayerScript : public PlayerScript
{
public:
    mod_foo_PlayerScript() : PlayerScript("mod_foo_PlayerScript") { }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        ChatHandler(player->GetSession()).SendSysMessage("Hello from mod-foo!");
    }
};

void AddSC_mod_foo()
{
    new mod_foo_PlayerScript();
}
```

`src/mod_foo_loader.cpp`
```cpp
void AddSC_mod_foo();

void Addmod_fooScripts()   // folder mod-foo -> Addmod_fooScripts
{
    AddSC_mod_foo();
}
```

That's a complete, buildable module.

---

## 3. Optional pieces

| Path | Purpose |
|---|---|
| `conf/mod_foo.conf.dist` | Config keys (read via `sConfigMgr`); auto-copied to `configs/modules/`. |
| `sql/world/*.sql` (and characters/auth/hotfixes) | Database changes (see install guide / module SQL section). |
| `mod-foo.cmake` | Extra CMake run inline at configure time (link libs, includes, defines). |
| `README.md` | Document your module. |

Example `mod-foo.cmake`:
```cmake
# Link an extra library into the static modules target:
# find_package(CURL REQUIRED)
# target_link_libraries(modules PUBLIC CURL::libcurl)
```

---

## 4. Build

From the repository root:

```bash
# Static (default): module compiled into worldserver
cmake -S . -B build -DTOOLS=0 -DMODULES=static
cmake --build build -j$(nproc)

# Dynamic: module becomes bin/scripts/libmodule_mod_foo.so (hot-reloadable)
cmake -S . -B build -DMODULES=dynamic
cmake --build build -j$(nproc)

# Per-module override (e.g. disable one module)
cmake -S . -B build -DMODULE_MOD_FOO=disabled
```

CMake prints a "Module configuration" graph showing where each module landed
(worldserver / its own library / disabled).

> Requirements (this core): CMake **4.3.2**, C++14, Boost **1.83**, OpenSSL
> **3.x**, MariaDB **10.6.3**. See the top-level `README.md`.

---

## 5. Run

1. Copy `configs/modules/mod_foo.conf.dist` → `mod_foo.conf` (the build also
   places the `.dist` there) and edit values.
2. Apply any SQL (see `HOW_TO_INSTALL_MODULES.md`).
3. Start `worldserver`. Static modules are always loaded; dynamic modules load
   from `bin/scripts/` and can be hot-reloaded.

---

## 6. Available hooks / APIs

Modules use the same `ScriptMgr` API as core scripts. The most-used base classes:
`PlayerScript`, `CreatureScript`, `WorldScript`, `CommandScript`, `ItemScript`,
`GameObjectScript`, `SpellScriptLoader`, `InstanceMapScript`, `AchievementCriteriaScript`,
`QuestScript`, and more — see `src/server/game/Scripting/ScriptMgr.h`.

A curated list of the important APIs (player, spells, talents, pathing, units,
maps, chat/commands, gossip, events, DB) is in **`Dev/ModuleAPI_Reference.txt`**.
