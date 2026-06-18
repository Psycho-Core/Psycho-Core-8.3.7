# Psycho_Core — Build & Run Quick-Start Checklist
A one-page tick list. Detailed steps in BUILD_GUIDE_WINDOWS.md / BUILD_GUIDE_LINUX.md.

## A. Before you build
- [ ] CMake ≥ 4.3.2 installed
- [ ] Compiler: VS 2019/2022 (Win) OR GCC 14.x/Clang (Linux)
- [ ] MariaDB 11.8.6 available (server to run it; client to build against)
- [ ] Boost 1.83 → dep/boost or system or C:/local/boost_1_83_0
- [ ] OpenSSL 3.5.x → dep/openssl or system or C:/Program Files/OpenSSL-3_5_6-Win64
- [ ] MariaDB client → dep/mysql or installed

## B. Configure + build
- [ ] `mkdir build && cd build`
- [ ] `cmake .. -DTOOLS=1 -DSCRIPTS=static -DMODULES=static` (+ generator/arch on Win)
- [ ] Build ONCE (RelWithDebInfo). On error: fix → report → wait for "TRY AGAIN".

## C. Database (names + creds already set as defaults)
- [ ] Create DBs: Psycho_auth, Psycho_world, Psycho_characters, Psycho_hotfixes
- [ ] Create user Psycho / pass Psycho, GRANT ALL on the 4 DBs
- [ ] Import sql/base/*, let worldserver auto-update sql/updates/

## D. Runtime (server root, portable)
- [ ] Copy binaries + required DLLs (Win) into a SERVER ROOT folder
- [ ] Rename `worldserver.conf.dist` → `worldserver.conf` and `bnetserver.conf.dist` → `bnetserver.conf`
- [ ] Set `DataDir = "./data"`, `LogsDir = "./logs"` (no drive labels)
- [ ] Put extracted client data (dbc/maps/vmaps/mmaps/cameras/gt ≈ 10-15 GB) in ./data
- [ ] (mod-psychobot) set `Psychobot.Enable = 1` in `worldserver.conf` if you want bots

## E. First run
- [ ] Start bnetserver/authserver → then worldserver
- [ ] In worldserver console: `account create NAME PASS` ; `account set gmlevel NAME 3 -1`
- [ ] Point client realmlist at your server; log in.

## Reminders
- Paths in confs = relative to server root (./data, ./logs) — keeps the server portable.
- mod-psychobot keys live in `<server root>/worldserver.conf`; `conf-backup/mod_psychobot.conf.dist` is a restore/reference copy.
- DB creds default to Psycho/Psycho/Psycho_* (changed from bfa_*/root).
