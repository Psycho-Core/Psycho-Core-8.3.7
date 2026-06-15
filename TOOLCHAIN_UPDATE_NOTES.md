# Toolchain Update Notes

This workspace clone is based on `Titans-Project/Psycho_Core-8.3.7` at:

```text
bba77f6 Merge pull request #39 from Thordekk/main
```

Toolchain alignment target: `Psycho-Core/Psycho_Core-8.3.7`.

Aligned toolchain targets:

```text
CMake:   4.3.2
Boost:   1.83
OpenSSL: 3.5.x, Windows path target OpenSSL-3_5_6-Win64
MariaDB: 11.8.6 client search support
```

Files copied byte-for-byte from Psycho_Core toolchain:

```text
cmake/macros/FindBoost.cmake
cmake/macros/FindMySQL.cmake
cmake/macros/FindOpenSSL.cmake
dep/boost/CMakeLists.txt
dep/cotire/CMake/cotire.cmake
```

Files adapted for Psycho_Core while matching the Psycho toolchain intent:

```text
CMakeLists.txt              # CMake minimum set to 4.3.2; project remains Psycho_Core; modules not enabled yet
README.md                   # requirement notes updated to CMake 4.3.2 / Boost 1.83 / OpenSSL 3.5.x
```

OpenSSL/Boost.Asio runtime compatibility files previously brought from the Psycho_Core/Dio85 maintenance fork:

```text
src/common/Asio/Resolver.h
src/common/Cryptography/OpenSSLCrypto.cpp
src/common/Cryptography/OpenSSLCrypto.h
src/server/bnetserver/REST/LoginRESTService.cpp
src/server/shared/Realm/RealmList.cpp
src/server/shared/Realm/RealmList.h
src/server/worldserver/Main.cpp
```

Not done yet:

- Module system port.
- `mod-psychobot` port.
- SQL/database merge.
- Full compile/build. Per instruction, only CMake configure is allowed.

Correction after exact-alignment verification:

- Replaced the temporary Dio85 OpenSSL/Resolver runtime compatibility files with the matching Psycho_Core versions so the OpenSSL 3.x threading/no-op compatibility layer and resolver API match your current repo's approach.

## Configure-only test

CMake 4.3.2 was installed locally for this session via the Python `cmake==4.3.2` package under:

```text
/tmp/cmake-4.3.2-py
```

Configure command run:

```bash
cmake -S . -B configure-test \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DSCRIPTS=static \
  -DTOOLS=1 \
  -DWITH_WARNINGS=0 \
  -DWITH_COREDEBUG=0
```

No build command was run.

Configure result:

```text
CMake configure exit code: 1
```

Successful dependency detections before failure:

```text
CMake 4.3.2
GCC/G++ 14.2.0
OpenSSL 3.5.5
MariaDB client 11.8.6
MySQL/MariaDB library: /usr/lib/x86_64-linux-gnu/libmariadbclient.so
MySQL/MariaDB headers: /usr/include/mariadb
Git 2.47.3
Threads found
```

Configure stopped because Boost 1.83 is not installed in this sandbox:

```text
CMake Error at cmake/macros/FindBoost.cmake:179 (message):
  Could not find Boost. Boost headers not found. Set BOOST_ROOT to your
  Boost 1.83 folder, e.g. C:/local/boost_1_83_0.
```

Next required local step before configure can complete:

```text
Install/extract Boost 1.83 and set BOOST_ROOT to that path.
```

Psycho_Core-specific OpenSSL configure fix:

- `cmake/macros/FindOpenSSL.cmake` now prefers `OPENSSL_ROOT_DIR` include/lib paths on Unix before falling back to system OpenSSL. This is needed so Linux configure can target the local OpenSSL 3.5.6 extraction.

## Final configure-only test with requested versions

Local dependency paths used for configure only:

```text
CMake 4.3.2:   /tmp/cmake-4.3.2-py
Boost 1.83:    /tmp/boost183-root/usr
OpenSSL 3.5.6: /tmp/openssl356-root/usr
MariaDB:       system MariaDB client 11.8.6
```

Configure command:

```bash
BOOST_ROOT=/tmp/boost183-root/usr \
BOOST_LIBRARYDIR=/tmp/boost183-root/usr/lib/x86_64-linux-gnu \
OPENSSL_ROOT_DIR=/tmp/openssl356-root/usr \
LD_LIBRARY_PATH=/tmp/openssl356-root/usr/lib/x86_64-linux-gnu:/tmp/boost183-root/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH \
cmake -S . -B configure-test \
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

Configure result:

```text
CMake configure exit code: 0
Found Boost 1.83.0
Detected OpenSSL version: 3.5.6
MariaDB client version: 11.8.6
Module configuration: static
Script configuration: static
```

No build command was run.
