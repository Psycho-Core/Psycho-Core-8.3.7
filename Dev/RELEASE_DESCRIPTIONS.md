# Release Descriptions — Ready to Paste

Copy the text below for each release and paste it into the GitHub release
"Describe this release" box. Replace the existing garbage HTML.

---

## V1.0-Boost — Pre-compiled Boost

```
# Pre-compiled Boost 1.83.0 for Psycho-Core 8.3.7

Pre-built Boost libraries for MSVC 14.3 (Visual Studio 2022) x64.
Required to compile the server. Pick ONE option:

## Option 1: Release-Only (recommended — 25 MB)
**File:** `boost_dep_release.zip`
Extract into `dep/boost/` so you get:
```
dep/boost/boost/version.hpp      (headers)
dep/boost/stage/lib/             (8 Release .lib files)
```
This is all most users need for a Release build.

## Option 2: Release + Debug (280 MB)
**File:** `boost_dep.zip`
Extract into `dep/boost/` so you get:
```
dep/boost/boost/version.hpp
dep/boost/lib64-msvc-14.3/       (Release + Debug libs)
```
Use this if you need Debug builds.

## Option 3: Official Installer (196 MB)
**File:** `boost_1_83_0-msvc-14.3-64.exe`
Run the installer, then copy its `boost\` and `lib64-msvc-14.3\` folders into `dep/boost\`.

---

## After extraction, verify:
```
dep/boost/boost/version.hpp    MUST exist at this exact path
```
CMake auto-detects Boost in `dep/boost/` — no manual configuration needed.

Full instructions: `dep/boost/INSTALL_BOOST.txt`
```

---

## DB-1 through DB-4 — World Database

**Same description for all 4 (DB-1, DB-2, DB-3, DB-4):**

```
# Psycho-Core World Database (Part X of 4)

**Import order: DB-1 → DB-2 → DB-3 → DB-4**

These contain the full `psycho_world` database (split into 4 parts for GitHub's file size limit).

## Before importing:
1. Create the databases first:
   ```
   dep\mysql\setup_psycho_databases.bat
   ```
2. Start MySQL:
   ```
   dep\mysql\start_mysql.bat
   ```

## To import (for each part, in order):
```
mysql\bin\mysql.exe -u psycho -p -h 127.0.0.1 -P 3307 psycho_world < DB-X.sql
```
Password: `psycho`

Or use the console:
```
dep\mysql\mysql_console.bat
```
Then:
```sql
USE psycho_world;
SOURCE C:/path/to/DB-X.sql;
```

## Database layout:
| Database | Contents |
|----------|----------|
| `psycho_auth` | Account/login data (from `sql/base/1_auth.sql`) |
| `psycho_characters` | Character data (from `sql/base/2_characters.sql`) |
| `psycho_world` | **← These 4 files go here** (creatures, quests, spawns, etc.) |
| `psycho_hotfixes` | Hotfix data |

## Connection string (already in worldserver.conf):
```
WorldDatabaseInfo = "127.0.0.1;3307;psycho;psycho;psycho_world"
```
```

---

## How to update the descriptions on GitHub:

1. Go to: https://github.com/Psycho-Core/Psycho-Core-8.3.7/releases
2. Click **Edit** (pencil icon) on each release
3. Delete the old description
4. Paste the new Markdown text above
5. Click **Update release**

For DB-1, change "Part X of 4" to "Part 1 of 4" and "DB-X" to "DB-1".
For DB-2, change to "Part 2 of 4" and "DB-2". Etc.
