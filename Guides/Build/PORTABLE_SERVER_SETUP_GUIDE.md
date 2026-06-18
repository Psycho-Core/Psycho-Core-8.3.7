# Psycho-Core 8.3.7.35662 — PORTABLE Server Setup Guide
**Build:** 8.3.7.35662 · **Rule:** every path is RELATIVE to the SERVER ROOT — NO drive letters.

> PORTABILITY PRINCIPLE
> The "server root" = the folder that contains `worldserver` (+ `authserver`/`bnetserver`),
> `configs/`, `data/`, `lua_scripts/`, etc. Everything is addressed relative to THAT folder, so
> the whole server can be zipped and moved to any machine/drive and still run. We never write
> `C:\...` or `/home/...` — only `./` paths from the server root, or the CMake `dep/` bundled paths.

================================================================================
## PORTABLE LAYOUT (the shape to aim for)
================================================================================
```
<SERVER ROOT>/
├── worldserver(.exe)
├── authserver(.exe)         (or bnetserver.exe on this BFA core)
├── worldserver.conf         (copied from .dist, edited)
├── bnetserver.conf          (copied from .dist, edited)
├── conf-backup/
│   └── mod_psychobot.conf.dist  (backup reference only; live Psychobot keys are in worldserver.conf)
├── data/                    <- DataDir (extracted client data)
│   ├── dbc/  maps/  vmaps/  mmaps/  cameras/  gt/
├── lua_scripts/             <- (only if Eluna is added later)
└── logs/
```

### Portable config values (put these in worldserver.conf — note: NO drive labels)
```
DataDir   = "./data"
LogsDir   = "./logs"
SourceDirectory = "."          # only if you keep source beside the build; else leave default
# DB (already set as defaults in the .dist):
LoginDatabaseInfo     = "127.0.0.1;3306;Psycho;Psycho;Psycho_auth"
WorldDatabaseInfo     = "127.0.0.1;3306;Psycho;Psycho;Psycho_world"
CharacterDatabaseInfo = "127.0.0.1;3306;Psycho;Psycho;Psycho_characters"
HotfixDatabaseInfo    = "127.0.0.1;3306;Psycho;Psycho;Psycho_hotfixes"
```
> `./data` and `./logs` resolve relative to wherever you launch worldserver from — fully portable.
> Best practice: launch worldserver from the server root (or a launcher .bat that `cd`s there first).
> Psychobot settings are edited in `worldserver.conf` on this core; `conf-backup/mod_psychobot.conf.dist` is only a pristine reference copy.

### Dependency portability (source tree)
The CMake Find macros search the bundled `dep/` folders FIRST (relative to repo root, no drive
label), then fall back to system installs:
- `dep/mysql/`   <- unzip MariaDB 11.8.6 client here (or `dep/mysql/mariadb-11.8.6-winx64/`)
- `dep/openssl/` <- unzip OpenSSL 3.5.x here  (or `dep/openssl/OpenSSL-Win64/`)
- `dep/boost/`   <- Boost 1.83 here (or set BOOST_ROOT / use C:/local/boost_1_83_0)
So you can clone the repo to ANY path/drive, drop the deps into dep/, and configure works.
