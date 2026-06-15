# Psycho-Core 8.3.7.35662 — LINUX Build Guide (step-by-step)
**Target:** Linux x64 · GCC 14.2 (tested) · CMake 4.3.2 · Boost 1.83 · OpenSSL 3.5.x · MariaDB 11.8.6
**Build:** 8.3.7.35662 · Portable paths from server root (no absolute paths required at runtime).

> Per CRITICAL_BUILD_RULE.txt: build once; on error, fix + report + WAIT for "TRY AGAIN".
> (README notes Windows is the user's deploy target; Linux is also fully supported / compile-test.)

## 0. Prerequisites (Debian/Ubuntu example)
```
sudo apt update
sudo apt install -y git clang cmake make gcc g++ \
  libmysqlclient-dev libssl-dev libbz2-dev libreadline-dev libncurses-dev \
  libboost-all-dev zlib1g-dev
```
- Ensure **CMake ≥ 4.3.2** (apt may be older — install from cmake.org or Kitware APT if so).
- Ensure **GCC 14.x** (or a C++11-capable modern GCC/Clang).
- **MariaDB 11.8.6** server + client dev headers (or distro mariadb dev package).
- **Boost 1.83** and **OpenSSL 3.5.x** (distro packages, or extract into dep/ for portability).

## 1. Get the source
```
git clone https://github.com/Psycho-Core/Psycho-Core-8.3.7
cd Psycho-Core-8.3.7
```

## 2. Dependencies (portable option)
System packages are simplest on Linux. For a portable tree you may also place:
- OpenSSL 3.5.x -> `dep/openssl/` (macro searches `${CMAKE_SOURCE_DIR}/dep/openssl` first)
- Boost 1.83    -> `dep/boost/` or system libboost
- MariaDB client-> `dep/mysql/` or system libmysqlclient-dev
The Find macros prefer dep/ then fall back to /usr paths.

## 3. Configure (out-of-source build dir inside repo)
```
mkdir build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DTOOLS=1 -DSCRIPTS=static -DMODULES=static \
  -DCMAKE_INSTALL_PREFIX=../run
```
- `-DCMAKE_INSTALL_PREFIX=../run` keeps the installed server INSIDE the repo (portable).
- Add hints only if needed: `-DBOOST_ROOT=../dep/boost -DOPENSSL_ROOT_DIR=../dep/openssl`.

## 4. Build + install
```
make -j$(nproc)
make install
```
> First build can take 30-90+ min depending on cores. BUILD ONCE — on failure stop, report, fix, wait.

## 5. Server root (portable)
After `make install`, the server lands in `../run/` (your INSTALL_PREFIX):
```
run/
├── bin/{worldserver, bnetserver}
├── etc/{worldserver.conf.dist, bnetserver.conf.dist, modules/mod_psychobot.conf.dist}
```
Copy each `*.conf.dist` -> `*.conf`. Treat `run/` (or wherever you place the binaries) as the
SERVER ROOT and use relative paths in the conf.

## 6. Databases (MariaDB)
```
CREATE DATABASE Psycho_auth;       CREATE DATABASE Psycho_world;
CREATE DATABASE Psycho_characters; CREATE DATABASE Psycho_hotfixes;
CREATE USER 'Psycho'@'localhost' IDENTIFIED BY 'Psycho';
GRANT ALL PRIVILEGES ON Psycho_auth.*       TO 'Psycho'@'localhost';
GRANT ALL PRIVILEGES ON Psycho_world.*      TO 'Psycho'@'localhost';
GRANT ALL PRIVILEGES ON Psycho_characters.* TO 'Psycho'@'localhost';
GRANT ALL PRIVILEGES ON Psycho_hotfixes.*   TO 'Psycho'@'localhost';
FLUSH PRIVILEGES;
```
Import `sql/base/`, then the worldserver auto-updater applies `sql/updates/`.
(Defaults in the .conf already use Psycho/Psycho/Psycho_*.)

## 7. Client data
Place extracted `dbc maps vmaps mmaps cameras gt` into a `data/` folder beside the binaries and set
`DataDir = "./data"` in worldserver.conf (portable). Full data set ≈ 10-15 GB.

## 8. Extract client data on Linux (if you have a client)
Run the built tools from inside the WoW client folder (where .build.info + Data/ live):
```
<build>/bin/mapextractor
<build>/bin/vmap4extractor && <build>/bin/vmap4assembler Buildings vmaps
<build>/bin/mmaps_generator
```
then move dbc/maps/vmaps/mmaps/cameras/gt into your server's data/.

## 9. Run
```
./bnetserver &
./worldserver
```
In the worldserver console: `account create <name> <pass>` ; `account set gmlevel <name> 3 -1`.

## Common Linux gotchas
- CMake too old -> install ≥4.3.2 from Kitware.
- MySQL/OpenSSL not found -> install -dev packages or populate dep/.
- mmaps generation is slow (hours) — expected.
