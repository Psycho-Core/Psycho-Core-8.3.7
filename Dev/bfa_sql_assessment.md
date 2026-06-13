# BfaCore/Dio85 SQL Assessment

Date: 2026-06-13

## Short answer

No — the public `BfaCore-Reforged` / `dio85` SQL trees are **not complete DB packages**.

They are source repos with only:

- auth base SQL,
- characters base SQL,
- a small set of world/hotfix/characters update files.

They do **not** include a full world DB base dump or full hotfix DB base dump.

---

## SQL file counts and sizes

| Repo | SQL files | SQL size | Has full world DB base? | Has full hotfix DB base? |
|---|---:|---:|---:|---:|
| Titans BfaCore Reforged | 36 | ~1.1 MB | No | No |
| Dio85 BfaCore | 36 | ~1.1 MB | No | No |
| Bloodtigress BfaCore | 36 | ~1.1 MB | No | No |
| Legends-of-Azeroth-BFA | 269 | ~33 MB | No full base, but many useful updates | No full base |
| Psycho_Core | 6707 | ~160 MB | Carries `TDB_full_837.20101_2020_10_20.7z` | Has dev hotfix SQL too |
| AshamaneCore | 13149 | ~2.0 GB | Yes, but schema/branch compatibility must be checked | Yes, but schema/branch compatibility must be checked |

---

## BfaCore Reforged SQL contents

`Titans-Project/BfaCore-Reforged` contains mainly:

```text
sql/base/1_auth.sql
sql/base/2_characters.sql
sql/updates/world/*.sql
sql/updates/hotfixes/*.sql
sql/updates/characters/*.sql
```

Largest files are only around 100 KB except the bundled Windows SQL splitter executable.

This is not enough to stand up a complete populated BFA world by itself.

---

## Dio85 vs original Titans BfaCore SQL

Dio85 and Titans have the same SQL file count.

Only these SQL files differ:

```text
sql/base/1_auth.sql
sql/base/2_characters.sql
```

Dio85 does **not** add a world DB or hotfix DB base.

Therefore:

- Dio85 is not better for SQL completeness.
- Original Titans is not better for SQL completeness either.
- The choice between Dio85 and Titans is a **source/toolchain/runtime** choice, not a DB completeness choice.

---

## Important: Psycho has the missing full 8.3.7 DB archive

Your Psycho core contains:

```text
sql/base/TDB_full_837.20101_2020_10_20.7z
```

This appears to be the public TrinityCore 8.3.7 full DB archive naming:

```text
TDB_full_837.20101_2020_10_20.7z
```

This is likely the missing full base DB asset that the public BfaCore/Dio85 source repos do not carry.

---

## Revised recommendation

### Source base

Use either:

1. `dio85/BfaCore-Reforged` if it compiles cleanly and its OpenSSL/Azerite/DB2 changes help your toolchain.
2. `Titans-Project/BfaCore-Reforged` if you want the cleaner original public BfaCore base and plan to port your own toolchain work.

### DB base

Do **not** rely on BfaCore/Dio85 repo SQL alone.

Use:

1. Your existing `TDB_full_837.20101_2020_10_20.7z` as the full 8.3.7 world/hotfix starting point.
2. BfaCore/Dio85 auth + characters base SQL for auth/characters if required by the chosen source.
3. BfaCore/Dio85 small update set.
4. Legends-of-Azeroth-BFA SQL updates as primary content donor.
5. Ashamane SQL only selectively, with schema conversion.

---

## Practical DB plan

Recommended bootstrapping order:

1. Create clean databases:
   - `auth`
   - `characters`
   - `world`
   - `hotfixes`

2. Import auth/characters from chosen BfaCore base:

```text
sql/base/1_auth.sql
sql/base/2_characters.sql
```

3. Extract/import the full 8.3.7 DB archive:

```text
sql/base/TDB_full_837.20101_2020_10_20.7z
```

4. Apply chosen BfaCore updates.

5. Boot worldserver and fix DBErrors before adding donor SQL.

6. Apply Legends SQL packs one group at a time.

7. Use Ashamane SQL only as selective donor/reference.

---

## Corrected verdict

BfaCore/Dio85 is still a strong **source code base**, but its repo SQL is incomplete.

Your Psycho repo is actually stronger as a **DB asset holder** because it carries the full 8.3.7 TDB archive.

Best combined route:

```text
Source base: BfaCore / Dio85 or Titans
DB base: Psycho's TDB_full_837.20101_2020_10_20.7z + BfaCore auth/characters
Content donor: Legends SQL, then Ashamane selectively
Custom systems: Psycho modules + mod-psychobot
```
