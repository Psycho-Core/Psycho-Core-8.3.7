# Eluna → BfaCore-Reforged (8.3.7) — Porting Research Report
**Date:** 2026-06-14
**Core:** Titans-Project/BfaCore-Reforged (BFA 8.3.7.35662)
**Eluna:** github.com/ElunaLuaEngine/Eluna (branches: `master`, `tc-retail`, `cmangos-spell-update`)
**Purpose:** Research the real porting effort + fact-check your `Eluna_Porting_Guide_BfaCore_8.3.7.txt`.

---

## EXECUTIVE SUMMARY

Porting Eluna to BfaCore-Reforged is **possible but a genuine porting project**, not a drop-in.
Your existing guide has the right *spirit* and correct *phases conceptually*, but several
**specific paths, file names, and the C++ standard are inaccurate** versus the real Eluna repo and
your real core. I verified everything below by cloning both repos.

**The single biggest reality:** there is **NO official Eluna fork for BFA**. Eluna officially
maintains **only `ElunaTrinityWotlk` (3.3.5)**. `ElunaTrinityCata/Master/Bfa/Retail` all return 404.
So you cannot "git pull a ready Eluna-BFA" — you must **merge the standalone Eluna engine into your
core by hand and adapt it to the 8.3.7 API.** That's exactly why this is a backport.

---

## FACT-CHECK OF YOUR GUIDE (verified against the live repos)

| Your guide says | Reality (verified) | Verdict |
|---|---|---|
| Drop Eluna into `src/server/scripts/Custom/Eluna` | Eluna is normally a **submodule in `LuaEngine/`** at the core's `dep/`-level, wired via CMake. Putting it under `scripts/Custom/` can work but is non-standard and complicates the build. | ⚠️ Works but not the intended layout |
| Set `CMAKE_CXX_STANDARD 17` | **Eluna requires only C++11** (its own INSTALL.md: "Eluna uses C++11"). **Your core is built on C++11** (`ConfigureBaseTargets.cmake`: `-std=c++11`). Forcing C++17 is unnecessary and may fight your core's toolchain. | ❌ Incorrect — C++11 |
| Edit `src/server/game/Scripting/ScriptLoader.cpp` | Your core has **NO** such file. It uses a generated `src/server/scripts/ScriptLoader.cpp.in.cmake` + `ScriptLoader.h`. Eluna hooks register differently (via its own `LoadElunaScripts`/ScriptMgr integration). | ❌ Wrong path |
| Eluna files in `Eluna/src/LuaEngine/` and `Eluna/src/Hooks/` | Eluna has **no `src/` folder**. Files sit at repo **root** (`LuaEngine.cpp`, `ElunaMgr.cpp`, etc.). Hooks live in **`hooks/`**, methods in **`methods/TrinityCore/`**. | ❌ Wrong paths |
| Hook files named `ElunaPlayerScript.cpp`, `ElunaCreatureScript.cpp`, `ElunaGuildScript.cpp` | Real hook files: **`PlayerHooks.cpp`, `CreatureHooks.cpp`, `GuildHooks.cpp`** (+ GameObject, Gossip, Group, Item, Instance, Packet, Server, Spell, Vehicle, BattleGround). The hook signatures to match are in **`hooks/Hooks.h`**. | ❌ Wrong file names (right concept) |
| Method wrappers in `Eluna/src/LuaEngine/Methods/` (`UnitMethods.h`, `PlayerMethods.h`, `GlobalMethods.h`) | Real location: **`methods/TrinityCore/`** with exactly those file names (UnitMethods.h, PlayerMethods.h, GlobalMethods.h, +20 more). | ⚠️ Right files, wrong folder |
| Add `sEluna->Update(diff)` in `World::Update` | Correct concept — Eluna's update must be driven. `World.cpp` exists in your core. ✅ (exact call name may differ in current Eluna; verify against `LuaEngine.h`). | ✅ Correct |
| Phase 3 ObjectGuid conflicts | Real and valid concern. `ObjectGuid.h` exists in your core. The specific method names (`HasEntry`, `GetEntry`, `GetCounter`) must be checked against your 8.3.7 header. | ✅ Valid (verify per-method) |
| `eluna.conf.dist` + `lua_scripts` folder | Correct deployment step. Eluna ships a config; `lua_scripts/` next to worldserver is right. Modern Eluna also has a `modules/` system that compiles to `lua_scripts/modules`. | ✅ Correct |

---

## WHAT ELUNA ACTUALLY LOOKS LIKE (tc-retail branch, verified)

```
Eluna/ (repo root — NO src/ folder)
├── LuaEngine.cpp/.h         ← the engine core
├── ElunaMgr.cpp/.h          ElunaLoader.cpp/.h   ElunaConfig.cpp/.h
├── ElunaCompat.cpp/.h       ElunaTemplate / ElunaUtility / ElunaEventMgr
├── ElunaSpellWrapper.*      ElunaInstanceAI.*    ElunaCreatureAI.h
├── BindingMap.h  LuaValue.*  lmarshal.*
├── hooks/                   ← EVENT HOOKS (the Phase-4 work)
│   ├── PlayerHooks.cpp  CreatureHooks.cpp  GuildHooks.cpp  GameObjectHooks.cpp
│   ├── GossipHooks.cpp  GroupHooks.cpp  ItemHooks.cpp  InstanceHooks.cpp
│   ├── PacketHooks.cpp  ServerHooks.cpp  SpellHooks.cpp  VehicleHooks.cpp
│   ├── BattleGroundHooks.cpp  HookHelpers.h  Hooks.h  ← signatures to match
├── methods/                 ← LUA API WRAPPERS (the Phase-5 work)
│   ├── TrinityCore/  (UnitMethods.h, PlayerMethods.h, GlobalMethods.h, +20)
│   ├── CMangos/  Mangos/  VMangos/  AzerothCore/  Custom/
├── modules/   extensions/   docs/  (INSTALL.md, MERGING.md, USAGE.md)
└── CMakeLists.txt
```

**Targeting:** `tc-retail` is built to track modern retail/master TrinityCore. `master` Eluna +
the `methods/TrinityCore` set are the closest to what you adapt. Because there's no BFA fork, you
pick whichever Eluna branch compiles closest and **backport the deltas to 8.3.7**.

---

## THE REAL PORTING WORK (corrected phase plan)

### Phase 1 — Placement & CMake  *(corrected)*
- Put Eluna at **`src/server/game/LuaEngine/`** (mirrors how ElunaTrinityWotlk does it) OR keep
  your guide's `scripts/Custom/Eluna` if you prefer — but you must add Eluna's sources to the
  **game** library's CMake, not just `add_subdirectory`, because Eluna calls deep into game objects.
- Add the Lua dependency: your core must build/link **Lua 5.2** (Eluna targets Lua 5.2 / LuaJIT).
  Check `dep/` for an existing lua; if absent, add one. This is a real dependency step the guide omits.
- **C++ standard: leave at C++11** (your core's standard; Eluna supports it). Do NOT force C++17.

### Phase 2 — ScriptMgr / loader integration  *(corrected path)*
- Your core has no `game/Scripting/ScriptLoader.cpp`. Instead, hook Eluna into the core's
  **ScriptMgr** lifecycle (the WotLK fork adds `Eluna` calls inside `ScriptMgr.cpp` and a global
  `sEluna`). Add the engine init/teardown there, and register the hook scripts through Eluna's own
  loader rather than a single `AddSC_Eluna()` in a nonexistent file.

### Phase 3 — ObjectGuid API  *(valid)*
- Compare Eluna's GUID calls vs `src/server/game/Entities/Object/ObjectGuid.h`. Map any
  `GetEntry/GetCounter/HasEntry`-style mismatches to your 8.3.7 accessors. Real, expected work.

### Phase 4 — Hook signatures  *(valid; corrected file names)*
- Match each hook in **`hooks/*.cpp`** to your `ScriptMgr.h` signatures (e.g. `OnLogin(Player*, bool firstLogin)`).
  Edit `PlayerHooks.cpp`, `CreatureHooks.cpp`, etc. — not "ElunaPlayerScript.cpp".
- This is the biggest chunk: BFA→retail hook signatures diverge (param structs vs explicit args).

### Phase 5 — Method wrappers  *(valid; corrected folder)*
- Fix API mismatches in **`methods/TrinityCore/`** (UnitMethods.h auras, PlayerMethods.h gossip,
  GlobalMethods.h DB). BFA 8.3.7 signatures differ from retail — expect to adjust AddAura, gossip
  menu, and async query calls to your `DatabaseEnv.h`.

### Phase 6 — Update loop  *(valid)*
- Drive Eluna from `World::Update(diff)` (verify the exact method name in current `LuaEngine.h`;
  newer Eluna may use `sEluna->UpdateEluna(diff)` or per-state updates). Concept is correct.

### Phase 7 — Deployment  *(valid)*
- Ship `lua_scripts/` next to worldserver, add the Eluna config block to worldserver.conf (modern
  Eluna folds settings into the main conf rather than a separate eluna.conf in some builds — check
  what your merged build generates), and the `modules/` compiled output.

---

## DIFFICULTY & EFFORT ASSESSMENT

| Aspect | Rating | Notes |
|---|---|---|
| Overall difficulty | 🔴 **Hard** | Real C++ backport; no ready BFA fork exists |
| Time estimate | days–weeks | Depends on how many hook/method signatures diverge |
| Lua dependency setup | ⭐⭐ medium | Must ensure Lua 5.2 builds/links in your core |
| Hook signature alignment (Phase 4) | ⭐⭐⭐ biggest task | ~14 hook files to reconcile vs ScriptMgr.h |
| Method API fixes (Phase 5) | ⭐⭐⭐ ongoing | ~23 method headers; fix as compiler errors surface |
| Risk | medium-high | Mis-wired Update loop = memory growth/crash (your guide's note is correct) |

**Smartest shortcut:** study **ElunaLuaEngine/ElunaTrinityWotlk** — it's the *only* official
Eluna+TrinityCore merge. Diff its `git log`/changed files against vanilla TC 3.3.5 to see the
**exact set of core edits** Eluna needs (ScriptMgr, World, ObjectMgr, CMake, config). Then apply
the analogous edits to your 8.3.7 core. That diff is your real "what to change" checklist —
far more reliable than guessing.

---

## CORRECTIONS TO PUT IN YOUR GUIDE (summary)
1. **C++ standard = C++11, not C++17** (both Eluna and your core use C++11).
2. **Eluna has no `src/` folder** — files are at root; hooks in `hooks/`, methods in `methods/TrinityCore/`.
3. **Hook files are `PlayerHooks.cpp` / `CreatureHooks.cpp` / `GuildHooks.cpp`**, not `Eluna*Script.cpp`.
4. **Your core has no `game/Scripting/ScriptLoader.cpp`** — integrate via ScriptMgr + Eluna's loader; loader is `scripts/ScriptLoader.cpp.in.cmake` (generated).
5. **Add the Lua 5.2 dependency** + link it (missing from the guide entirely).
6. **No official Eluna-BFA fork exists** — use ElunaTrinityWotlk as the reference diff; backport by hand.
7. Verify the exact **Update call name** and **config mechanism** against current `LuaEngine.h`.

---

## BOTTOM LINE
Your guide's 7-phase structure is conceptually sound and worth keeping — but treat it as a *map*,
not exact instructions: the **C++17 claim, the file paths, and the hook file names are wrong**, and
it omits the **Lua dependency** and the fact that **no BFA Eluna fork exists**. The realistic path
is: add Lua 5.2 → merge Eluna engine into the game lib → wire ScriptMgr + World::Update → reconcile
`hooks/*.cpp` and `methods/TrinityCore/*.h` against your 8.3.7 API, using **ElunaTrinityWotlk's diff
as the authoritative reference** for which core files to touch.
