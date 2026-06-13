<!--
===========================================================================
 Copyright (c) 2026 Psycho-core. All rights reserved.
 Original work authored 100% from scratch for Psycho_Core.
 Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 NOT covered by the base GPL framework license. Development/evaluation only.
===========================================================================
-->

# mod-psychobot

Player-driven combat bots for **Psycho_Core 8.3.7** (TrinityCore BfA).

Built on the Psycho_Core module system; AI uses a clean-room ike3-style
Strategy / Action / Value / Trigger engine.

---

## What it does

Turn any offline character on your account into an AI-controlled combat bot
that follows you, fights by your side, and uses real class rotations with
BfA-accurate talents and specializations.

- **12 classes supported** — Death Knight, Demon Hunter, Druid, Hunter, Mage,
  Monk, Paladin, Priest, Rogue, Shaman, Warlock, Warrior (all 36 specs).
- **Real talent trees** — BfA 8.3.x 7×3 talent grid applied automatically per spec.
- **Smart rotations** — Priority-based spell casting (damage, healing, tanking)
  via name resolution against live spell data. Wrong names no-op safely.
- **Group / Raid aware** — Bots join your party, respect LFG roles, assist
  targets, and heal allies.
- **Socketless login** — Offline alts are logged in without a real network
  connection (S28). No client needed per bot.
- **Strategy persistence** — Toggled strategies saved to the characters database
  and restored on relog.
- **S30 auto-spawn factory** — Can generate PSYCHOBOT accounts/characters from
  the `ai_playerbot_names` pool and feed them into the random-bot population.
- **AHBot scaffold** — Basic auction-house posting framework (not yet live trading).

---

## Requirements

| Component | Version | Notes |
|---|---|---|
| **Psycho_Core** | `8.3.7` | This module is built for the Psycho_Core BfA server. |
| **CMake** | `>= 4.3.2` | Same as core requirement. |
| **C++** | `C++14` | Same as core. |
| **Boost** | `1.83` | Same as core. |
| **OpenSSL** | `3.5.x` | Same as core. |
| **MariaDB** | `10.6+` / `11.8` | For the `psychobot_strategies` SQL table. |

> ⚠️ This module **does not** work with AzerothCore WotLK 3.3.5 or other
> TrinityCore forks unmodified. The API surface is BfA 8.3.x specific.

---

## Install

### 1. Clone into your core's `modules/` folder

```bash
cd /path/to/Psycho_Core-8.3.7/modules
git clone https://github.com/Psycho-core/mod-psychobot.git
cd ..
```

### 2. Reconfigure and build

```bash
cmake -S . -B bld -DMODULES=static
cmake --build bld -j$(nproc)
```

(The module auto-discovers; `MODULES=static` links it into `worldserver`.)

### 3. Apply the SQL

Run this once against your **auth/login** database so GM accounts can use the command tree:

```bash
mysql -u trinity -p auth < modules/mod-psychobot/sql/auth/psychobot_rbac.sql
```

Run this once against your **characters** database:

```bash
mysql -u trinity -p characters < modules/mod-psychobot/sql/characters/psychobot_strategies.sql
```

This creates `psychobot_strategies` for per-bot persistence.

If you enable S30 random bot generation / auto-spawn, also run this once against
your **world** database:

```bash
mysql -u trinity -p world < modules/mod-psychobot/sql/world/psychobot_names.sql
```

This creates/populates `ai_playerbot_names`, `ai_playerbot_guild_names`, and
`ai_playerbot_arena_team_names`; the S30 factory currently needs
`ai_playerbot_names`.

### 4. Configure

Copy the default config and enable:

```bash
cp bld/bin/configs/modules/mod_psychobot.conf.dist \
   /path/to/server/configs/modules/mod_psychobot.conf
```

Edit `mod_psychobot.conf`:

```ini
Psychobot.Enable = 1
```

### 5. Start worldserver

Bots are managed entirely in-game via chat commands (see below).

---

## In-game commands

| Command | Description |
|---|---|
| `.psychobot add <charname>` | Take control of an offline character as your bot. Socketless login + auto-follow. |
| `.psychobot remove <charname>` | Release the bot (socketless bots are logged back out). |
| `.psychobot list` | List all your active bots. |
| `.psychobot spec <charname> <0-3>` | Set the bot's spec + auto-apply BfA talents. |
| `.psychobot group <charname>` | Invite the bot to your party (LFG role auto-assigned). |
| `.psychobot follow` / `stay` / `attack` | Order all your bots (follow / hold / attack your target). |
| `.psychobot cast <spell name>` | All bots cast the named spell on your target. |
| `.psychobot strategy <name>` | Toggle a combat strategy on a bot (persisted in DB). |
| `.psychobot summon <charname>` | Summon an active bot to you. |
| `.psychobot factory spawn [count]` | Generate up to 50 bot characters immediately (S30). |
| `.psychobot factory purge` | Delete generated `PSYCHOBOT_%` accounts/chars through core delete paths. |
| `.psychobot help` | Print the full command grammar. |

---

## Architecture

```
mod-psychobot/
├── conf/
│   └── mod_psychobot.conf.dist       # Config keys
├── sql/
│   └── characters/
│       └── psychobot_strategies.sql  # Persistence table
├── src/
│   ├── mod_psychobot.cpp             # Module entry + CommandScript
│   ├── mod_psychobot_loader.cpp      # CMake loader hook
│   ├── PsychobotMgr.cpp/.h           # Bot manager (tick, add/remove, broadcast)
│   ├── PsychobotAI.cpp/.h            # Per-bot brain (~250ms tick)
│   ├── PsychobotLoginMgr.cpp/.h      # S28 socketless login
│   ├── PsychobotFactory.cpp/.h       # S30 account/character auto-spawn
│   ├── PsychobotGroupMgr.cpp/.h      # Party / LFG / role helpers
│   ├── PsychobotPopulationMgr.cpp/.h  # SmartScale / world behaviour
│   ├── PsychobotTalentMgr.cpp/.h     # BfA spec + talent grid
│   ├── PsychobotSpecRoles.cpp/.h     # Spec → role mapping
│   ├── PsychobotGearMgr.cpp/.h       # Gear upgrade framework
│   ├── PsychobotAhBot.cpp/.h         # Auction-house scaffold
│   ├── PsychobotDbStore.cpp/.h       # DB persistence (strategies)
│   ├── engine/                       # Engine, Action, Trigger, Strategy, Value
│   ├── ai/                           # AI factory, ServerFacade
│   ├── classes/                      # Per-class rotation files (12 classes)
│   ├── actions/                      # CastSpell, MoveTo, etc.
│   ├── triggers/                     # Health, Threat, Proc, etc.
│   ├── strategies/                   # Combat, healing, buff, etc.
│   ├── values/                       # Distance, Target, Health, etc.
│   ├── travel/                       # Pathing / waypoint stubs
│   ├── pvp/                          # BG / arena awareness
│   ├── dungeon/                      # Instance / encounter stubs
│   ├── pets/                         # Pet management
│   └── world/                        # Vendor, loot, gather, quest stubs
└── README.md                         # This file
```

---

## Honest gaps (not bugs)

These are documented framework hooks, not yet live:

- Vendor sell / buy / repair
- Loot pickup (corpse + game objects)
- Quest accept / complete
- Gathering (herb / mining / skinning)
- Live AH posting / buying
- Per-BG objective play
- Per-encounter boss reactions
- Flee / retreat behaviour
- Chat grammar / personality / LLM
- Performance / memory monitors

All are stubbed with clear extension points. See `Dev/FUTURE_MOD_PSYCHOBOT_UPGRADES.txt`
in the main core repo for the roadmap.

---

## License

This module is **original work** authored for Psycho_Core and covered by the
`LICENSE.MYCODE` source-available development license (see the file in the
repo root). The base TrinityCore framework remains under GPL-2.0-or-later.

> When you write your own module, put your own license header on files you
> create from scratch. Edits to existing core files stay under the base GPL.

---

## Links

- **Core repo:** https://github.com/Psycho-core/Psycho_Core-8.3.7
- **Build guide (Linux):** `Dev/BUILD_GUIDE_LINUX.txt` in core repo
- **Build guide (Windows):** `Dev/BUILD_GUIDE_WINDOWS.txt` in core repo
- **Future upgrades:** `Dev/FUTURE_MOD_PSYCHOBOT_UPGRADES.txt` in core repo

---

> **IMPORTANT REMINDER:** CURRENTLY UNDER DEVELOPMENT BY A COMPLETE NOOB WITH AN INTERNET CONNECTION!
