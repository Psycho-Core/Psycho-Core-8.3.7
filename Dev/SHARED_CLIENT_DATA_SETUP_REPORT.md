# Shared Client Data Setup — Research & How-To (BFA 8.3.7 / Psycho-Core)
**Date:** 2026-06-14
**Context:** Extraction tools (mapextractor / vmap4extractor / vmap4assembler / mmaps_generator)
can't find the client data folder/files. This report explains WHY, and how a "shared client
data" setup works, how hard it is, and the exact steps.

---

## TL;DR
- **Difficulty: EASY-to-MODERATE.** A shared client-data setup is genuinely simple — it's just
  one config line (`DataDir`) pointing the server at a folder of already-extracted data.
- **The hard/slow part is the one-time EXTRACTION** (and that's what's failing for you).
- **Best move:** extract ONCE (or get a pre-extracted 8.3.7 data pack), put the output on a
  shared/central location, and point every server build at it via `DataDir`. You never have to
  re-extract again.

---

## PART 1 — How client data actually works (so the fix makes sense)

The server does NOT read your WoW client directly. It reads **pre-processed files** the tools
extract from the client's CASC storage. There are 5 outputs:

| Folder | What it is | Needed? | Made by |
|--------|-----------|---------|---------|
| `dbc/` (8.x: contains DB2) | Data tables (races, maps, items…) | **Required** | mapextractor |
| `maps/` | Terrain height grids | **Required** | mapextractor |
| `cameras/` | Cinematic camera splines | Required | mapextractor |
| `vmaps/` | Line-of-sight / collision | Optional (strongly recommended) | vmap4extractor → vmap4assembler |
| `mmaps/` | NPC pathfinding/movement | Optional (strongly recommended) | mmaps_generator |

At runtime the server finds them through ONE config value in `worldserver.conf`:

```
DataDir = "."        # default = look in current dir
```

Point it anywhere and it loads from there. THIS is the entire "shared data" mechanism.

---

## PART 2 — WHY your tools "can't find the data" (root cause)

This is the #1 BFA-era extraction failure, and it's almost always ONE of these:

1. **`Error opening casc storage '...\Data': FILE_NOT_FOUND` / "No locales detected"**
   - **Cause: the client is missing its `.build.info` file** (top of the WoW folder).
     BFA clients pulled from torrents/p2p frequently have this stripped. The extractor reads
     `.build.info` to learn the build number + locale; with it gone, it sees nothing.
   - **Fix:** obtain/restore a valid `.build.info` for 8.3.7.35662 and place it at the WoW root.

2. **Run the tool from the WRONG directory.**
   - The extractor must be run **from inside the WoW client root** (the folder containing the
     `Data/` directory and `.build.info`), with the tool path pointed at it — NOT from the
     server/bin folder. e.g. `cd /path/to/WoW-8.3.7` then run `/path/to/server/bin/mapextractor`.

3. **Wrong client build for the tools.**
   - 8.3.7 tools need an **8.3.7 (35662)** client. A mismatched build (e.g. 8.0.1, or a
     Shadowlands client) makes the DB2/CASC layout unreadable.

4. **No locale / wrong locale folder.**
   - The `Data/` must contain a locale (e.g. `enUS`). Run the client to the login screen ONCE
     so it finalizes locale files before extracting.

5. **Empty/partial `Data/` (CASC not fully downloaded).**
   - If the client was never fully installed/patched, CASC has only stubs → FILE_NOT_FOUND.

> Your symptom ("tools can't find the data folder/files") = almost certainly #1 or #2.

---

## PART 3 — What a "Shared Client Data" setup IS, and is it worth it?

**Yes — it's the standard pro move.** Two flavours:

### A) Local shared folder (one machine, many server builds) — TRIVIAL
Extract once to a central folder, e.g. `/srv/wow-data/8.3.7/`, then EVERY core build's
`worldserver.conf` uses:
```
DataDir = "/srv/wow-data/8.3.7"
```
Rebuild/wipe your server as often as you like — the multi-GB data never moves. **This is what
you want.**

### B) Network shared folder (multiple machines) — EASY, with a caveat
Put the extracted data on a NAS / network share (SMB/NFS) and point each server's `DataDir`
at the mounted path:
```
DataDir = "/mnt/wow-share/8.3.7"          # Linux NFS/SMB mount
DataDir = "\\\\NAS\\wow\\8.3.7"           # Windows UNC (use a mapped drive for a service)
```
- **Works fine for `dbc`, `maps`, `vmaps`** (read-once-ish, cached).
- **`mmaps` are read frequently during play** — on a slow network share this can add latency /
  pathfinding hitches. For a busy realm, keep `mmaps` LOCAL and only share the rest, or use a
  fast (gigabit+) link. For a small/solo/dev server, fully-shared is fine.

### Difficulty rating
| Task | Difficulty | Time |
|------|-----------|------|
| Configure `DataDir` to a shared path | ⭐ Trivial | 1 minute |
| Local shared folder setup | ⭐ Trivial | 5 minutes |
| Network share (NAS/SMB/NFS) setup | ⭐⭐ Easy | 15–30 min |
| The ACTUAL extraction (your blocker) | ⭐⭐⭐ Moderate | mins→hours, ONCE |

---

## PART 4 — Recommended path forward (in order)

### Option 1 (FASTEST): Use a pre-extracted 8.3.7 data pack + shared DataDir
Skip extraction entirely. The data is build-specific but **not server-specific** — extracted
8.3.7.35662 `dbc/maps/vmaps/mmaps` works on ANY 8.3.7 TrinityCore-based core (incl. Psycho-Core).
- Get a complete 8.3.7 (35662) data set (community packs exist, same idea as the
  3.3.5 data repos like Torrer/TrinityCore-3.3.5-data, but for BFA).
- Drop it at `/srv/wow-data/8.3.7/` and set `DataDir` to it.
- **Verify build compatibility:** the server prints "incompatible clientversion" if the map
  files are from the wrong build — they MUST be 35662.

### Option 2 (PROPER): Fix the extractor, extract once, then share
1. Restore the **`.build.info`** file to your 8.3.7 client root (fixes the CASC error).
2. Confirm the client is **35662** and has a locale (run it to login screen once).
3. `cd` INTO the client folder; run the tools by full path, in order:
   `mapextractor` → `vmap4extractor` → `vmap4assembler Buildings vmaps` → `mmaps_generator`.
4. Move `dbc maps cameras vmaps mmaps` to `/srv/wow-data/8.3.7/`.
5. Set `DataDir = "/srv/wow-data/8.3.7"` in every core's `worldserver.conf`.

### Minimal config to get RUNNING fast (data on shared path)
```ini
DataDir = "/srv/wow-data/8.3.7"
# if you don't have mmaps/vmaps yet, you CAN boot without them (not recommended long-term):
mmap.enablePathFinding = 0
vmap.enableLOS = 0
vmap.enableHeight = 0
vmap.enableIndoorCheck = 0
```
(Turn these back to 1 once vmaps/mmaps are in place — otherwise NPCs walk through walls.)

---

## PART 5 — Quick diagnostic checklist for YOUR failing tools
Run these to pin down the cause:
- [ ] Does `.build.info` exist at the WoW client root? (if NOT → that's your bug)
- [ ] Is the client build exactly **8.3.7.35662**?
- [ ] Are you running the tool **from inside the client folder** (where `Data/` lives)?
- [ ] Does `Data/` contain a locale folder and real (large) CASC files, not just stubs?
- [ ] Did you run the client to the login screen at least once?
- [ ] Are the tools compiled from the **same 8.3.7 core** (yours), in Release mode?

---

## BOTTOM LINE
A **shared client-data setup is easy** — it's literally one `DataDir` line pointing at a central
folder, and it's the right architecture (extract once, reuse forever, across rebuilds/machines).
Your real blocker isn't the sharing — it's the **extraction failing**, which is almost certainly
a **missing `.build.info`** and/or **running the tool from the wrong directory**. Fix that once
(or grab a pre-extracted 35662 data pack), park the output on a shared path, and you're done.
