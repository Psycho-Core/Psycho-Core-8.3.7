# How the `.build.info` Was Derived — Psycho-Core 8.3.7.35662

**Purpose:** explain, step by step, exactly which Windows `cmd` commands were run, what the raw
CASC config files contained, and how that turned into a working `.build.info`. So any user can
reproduce it and understand *why* each value is what it is.

> **Verification status (read this first):**
> Every format claim in this document was checked against authoritative sources:
> - The CASC build-config / CDN-config field names are from the **wowdev.wiki TACT page**
>   (`build-name`, `build-uid`, `install`, `install-size`, `archive-group`, etc.).
> - The `.build.info` field layout and a **known-working 8.3.7 (35662) example** come from the
>   official **TrinityCore support forum** (a `.build.info` someone built for this exact client).
> - The CASC path layout (`config\XX\YY\<hash>`, `data.NNN`, `.idx`) is from wowdev.wiki CASC.
>
> Where something is an assumption (e.g. UI locale), it is explicitly labelled. Nothing here is
> invented. The raw `Data\config\` files were pasted into chat (not stored in this workspace),
> so the hash values are exactly what your dump showed — re-dump beats this doc if they differ.

---

## 1. Background — what these files are

Modern WoW (Warlords of Draenor 6.0 and later) uses **CASC** (Content Addressable Storage
Container) instead of the old MPQ archives. A CASC install is described by a small chain of
plain-text config files:

```
.build.info        <- top-level index at the client ROOT. Lists which build is active and the config keys.
   |
   +--> Build Config file   ("Build Key")  -> names the build; points at root/encoding/install/download
   |
   +--> CDN Config file     ("CDN Key")    -> lists the data archives ("archives" / "archive-group")
```

Those Build Config and CDN Config files live on disk under (note: the **filename is the hash**,
because CASC is content-addressed):

```
<client>\Data\config\<first 2 hex chars>\<next 2 hex chars>\<full 32-char hash>
```

Example: Build Config `01d99c975c422649fc50589978c9f90f` is stored at
`...\Data\config\01\d9\01d99c975c422649fc50589978c9f90f`.

**The problem:** this client was **missing `.build.info`** at its root. Local-mode tools
(CASCExplorer, CascView) need `.build.info` to bootstrap, so they refused with
"Local mode not supported for this game". The fix was to **reconstruct `.build.info` by hand**
from the config files that *were* present under `Data\config\`.

---

## 2. The Windows `cmd` commands, and what each one did

Run these from inside the client folder
(`D:\Blakes_SHIT\World of Warcraft - 8.3.7 35662\`).

### 2a. Confirm `.build.info` really is missing

```cmd
dir /a .build.info
```
- `dir` = list directory entries; `/a` = include **a**ll files, including hidden/system ones.
- Result here: **File Not Found** → it genuinely did not exist (not merely hidden).

Same check for the other launcher files:
```cmd
dir /a .build.db
dir /a .flavor.info
```
Both absent — consistent with a client shipped without Battle.net launcher metadata.
(`.build.info` and `.flavor.info` are normally written by the Battle.net Agent, not the game
client — which is why a hand-copied client can be missing them.)

### 2b. Confirm the real data archives are present

```cmd
dir Data\data
```
Listed `data.000` … `data.062` plus `.idx` index files (~67 GB). That confirms a **full, real
client** (not a streaming stub), so extraction is viable once a tool can open `Data`.

### 2c. List every config file (the filenames ARE the hashes)

```cmd
dir /s /b Data\config
```
- `/s` = recurse into **s**ubdirectories (config is nested two levels deep by hash prefix).
- `/b` = **b**are output — just full paths, one per line.

This printed the full path of every Build Config and CDN Config. Because the filename equals the
hash, this alone gave the candidate **Build Key** and **CDN Key** values.

### 2d. Print the contents of a config file

```cmd
type Data\config\01\d9\01d99c975c422649fc50589978c9f90f
```
- `type` = print a text file. Build/CDN configs are plain text in a simple `key = value` format.

You pasted those printouts into chat — that paste is the source of every value below.

---

## 3. What the config files contained, and how each value maps

### 3a. Build Config (the "Build Key")

Chosen primary Build Config: **`01d99c975c422649fc50589978c9f90f`**.

Inside a Build Config the relevant keys are (field meanings per wowdev.wiki TACT → Build Config):

| Key inside the file | Meaning (per wowdev.wiki) | Value from your dump | Used as |
|---|---|---|---|
| `build-name` | "Name of the build" | `WOW-35662patch8.3.7_Retail` | confirms build → `Version` = `8.3.7.35662` |
| `build-uid` | "Program code (see Products)" | `wow` | confirms product = `wow` (Retail) |
| `install` | first key = content hash of the decoded install file | `aec322c1a02af5246c3f20a3d383d2a3` | **Install Key** (optional — see §4) |
| `install-size` | install size for the install hash | `16875` | **IM Size** (optional — see §4) |

> The Build Config **filename** (`01d99c97…`) is the **Build Key** value. The file's *contents*
> give the install hash, install-size, and build name.

**Why `01d99…` over the other Build Configs you dumped:** it is the variant whose `build-name`
is the plain `..._Retail` build (`install-size 16875`). The others (`07dc…`, `407c…`, `8305…`,
`dde2…`, `fc4b…`) are alternate-locale variants (`install-size 16425`). If `01d99…` ever turns
out wrong, the documented fallback order is `07dc…` then `fc4b…`.

### 3b. CDN Config (the "CDN Key")

All six CDN Config variants you dumped were identical in the parts that matter, so the canonical
one is **`245f00b65d6d6c7cc7c52bab5c9e600c`**. Its **filename** is the **CDN Key**.
(Its contents list `archives` / `archive-group` — the data archive index. `.build.info` only
stores the *key*; the tool reads that file off disk itself.)

### 3c. Values that are NOT in the config files (conventions / region / locale)

Some `.build.info` columns are launcher metadata, not stored in Build/CDN configs:

| Column | Value used | Where it comes from |
|---|---|---|
| `Branch` | `us` | **region code**, NOT the product — see correction note below |
| `Active` | `1` | marks the active build (tools pick the entry where Active=1) |
| `CDN Path` | `tpr/wow` | Blizzard's standard WoW CDN path prefix |
| `CDN Hosts` | `level3.blizzard.com …` | standard Blizzard CDN hosts |
| `CDN Servers` | `http://level3.blizzard.com/?maxhosts=4 …` | standard server URL list |
| `Tags` | `Windows x86_64 … enUS …` | OS is known; **locale is assumed enUS** |
| `Version` | `8.3.7.35662` | from `build-name` |
| `Product` | `wow` | from `build-uid` (TACT product `wow` = WoW Retail) |
| `Armadillo`, `Last Activated` | empty | not needed for local extraction |

> **Correction / important accuracy note:** In real WoW `.build.info` files, the **`Branch`
> column holds a region/edition code such as `us` or `eu` — not the word `wow`.** (Verified
> against multiple real files, including a working TrinityCore-made 8.3.7 example.) The product
> name `wow` belongs in the **`Product`** column. Earlier guidance that mapped `build-uid` →
> `Branch` was imprecise; `build-uid` correctly maps to **Product**, and `Branch` should be a
> region like `us`.

> **Locale note:** the `Tags` column assumes **enUS**. If your client is another language, that
> is the field to change (e.g. `enGB`, `deDE`, `ruRU`). The Build/CDN config hashes do not encode
> the UI locale by themselves, so locale is always an assumption.

---

## 4. The `.build.info` format — and the version that actually works for 8.3.7

**The field set is NOT fixed — it varies by client version.** Real examples differ:
- A 6.0.3 file had **12** columns (no `CDN Servers`, no `KeyRing`, no `Product`).
- D2R / StarCraft files include a **`KeyRing!HEX:16`** column.
- A **TrinityCore-made 8.3.7 (35662)** `.build.info` uses **14** columns **with `Product` but
  WITHOUT `KeyRing`**.

For this 8.3.7 client, the safest, **verified-working** layout is the TrinityCore 8.3.7 one
(header line first, data line second; columns separated by `|`):

```
Branch!STRING:0|Active!DEC:1|Build Key!HEX:16|CDN Key!HEX:16|Install Key!HEX:16|IM Size!DEC:4|CDN Path!STRING:0|CDN Hosts!STRING:0|CDN Servers!STRING:0|Tags!STRING:0|Armadillo!STRING:0|Last Activated!STRING:0|Version!STRING:0|Product!STRING:0
us|1|01d99c975c422649fc50589978c9f90f|245f00b65d6d6c7cc7c52bab5c9e600c|aec322c1a02af5246c3f20a3d383d2a3|16875|tpr/wow|level3.blizzard.com eu.cdn.blizzard.com|http://eu.cdn.blizzard.com/?maxhosts=4 http://level3.blizzard.com/?maxhosts=4 https://blzddist1-a.akamaihd.net/?fallback=1&maxhosts=4 https://eu.cdn.blizzard.com/?fallback=1&maxhosts=4 https://level3.ssl.blizzard.com/?fallback=1&maxhosts=4|Windows x86_64 US? acct-ENG? geoip-US? enUS speech?:Windows x86_64 US? acct-ENG? geoip-US? enUS text?|||8.3.7.35662|wow
```

Header-line syntax: each column is `Name!TYPE:length`; `STRING:0` = text, `DEC:n` = decimal,
`HEX:16` = a 16-byte (32 hex char) hash. The data line repeats the same `|` order.

### A simpler, also-valid minimal form

Many working hand-made files **leave `Install Key` and `IM Size` blank** (`…||…`), because
CascLib does not strictly require them for local extraction. If the version above ever misbehaves,
this minimal variant is known to work too:

```
Branch!STRING:0|Active!DEC:1|Build Key!HEX:16|CDN Key!HEX:16|Install Key!HEX:16|IM Size!DEC:4|CDN Path!STRING:0|CDN Hosts!STRING:0|CDN Servers!STRING:0|Tags!STRING:0|Armadillo!STRING:0|Last Activated!STRING:0|Version!STRING:0|Product!STRING:0
us|1|01d99c975c422649fc50589978c9f90f|245f00b65d6d6c7cc7c52bab5c9e600c|||tpr/wow|level3.blizzard.com|http://level3.blizzard.com/?maxhosts=4|Windows x86_64 US? enUS speech?:Windows x86_64 US? enUS text?|||8.3.7.35662|wow
```

### Save instructions

Save as `.build.info` (leading dot, **no `.txt`**) at the client root. In Notepad:
File → Save As → **"Save as type: All Files"** → Encoding: **ANSI**.
After saving, run `dir /a .build.info` to confirm Windows didn't append `.txt`.

### Field-by-field map (14-column 8.3.7 layout)

| # | Column | Value | Source |
|---|---|---|---|
| 1 | Branch | `us` | region code (NOT product) |
| 2 | Active | `1` | convention (active build) |
| 3 | Build Key | `01d99c975c422649fc50589978c9f90f` | Build Config **filename** |
| 4 | CDN Key | `245f00b65d6d6c7cc7c52bab5c9e600c` | CDN Config **filename** |
| 5 | Install Key | `aec322c1a02af5246c3f20a3d383d2a3` | Build Config `install` 1st hash (optional) |
| 6 | IM Size | `16875` | Build Config `install-size` (optional) |
| 7 | CDN Path | `tpr/wow` | Blizzard standard |
| 8 | CDN Hosts | `level3.blizzard.com …` | Blizzard standard |
| 9 | CDN Servers | `http://… maxhosts=4 …` | Blizzard standard |
| 10 | Tags | `Windows x86_64 … enUS …` | OS known; **locale assumed enUS** |
| 11 | Armadillo | *(empty)* | not needed offline |
| 12 | Last Activated | *(empty)* | not needed offline |
| 13 | Version | `8.3.7.35662` | Build Config `build-name` |
| 14 | Product | `wow` | Build Config `build-uid` (TACT `wow` = Retail) |

---

## 5. IMPORTANT: you may not even need `.build.info`

After this file was created, CASCExplorer (local mode) still crashed — but that crash is a
**CASCExplorer bug** (NullReference in its CDN-index handler), not a `.build.info` problem. The
bootstrap actually succeeded.

More importantly, this core's **own extractor** — verified in
`src/tools/map_extractor/System.cpp` and `extractor_common/CascHandles.cpp`:

- opens `<client>\Data` directly via **CascLib** (`CascOpenStorageEx`), product `"wow"`;
- **does not read `.build.info` at all**; it reads the build number straight from CascLib.

So `.build.info` is **harmless to keep** (it helps other CascLib tools detect the build) but is
**not on the critical path** for your actual extraction. The real extraction is done by the tools
the core build produces (`mapextractor.exe`, etc.).

---

## 6. Quick reference — regenerate from scratch

```cmd
cd /d "D:\Blakes_SHIT\World of Warcraft - 8.3.7 35662"
dir /a .build.info                 :: confirm it's missing
dir Data\data                      :: confirm data.000..NNN present
dir /s /b Data\config              :: list all config hashes (filenames = hashes)
type Data\config\XX\YY\<hash>      :: read each; find build-uid / build-name / install / install-size
```

Then:
1. Pick the Build Config whose `build-name` is the plain `..._Retail` build → its filename = **Build Key**.
2. (Optional) read its `install` first hash → **Install Key**; `install-size` → **IM Size** (or leave blank).
3. Pick any CDN Config filename (they were identical) → **CDN Key**.
4. **Branch = a region code (`us`/`eu`)**, **Product = `build-uid` (`wow`)**, **Version from `build-name`**, **Active = `1`**.
5. Set `Tags` to your actual UI locale.
6. Save the two lines as `.build.info` (All Files, ANSI) at the client root; verify with `dir /a .build.info`.

---

## 7. Sources used to verify this document

- wowdev.wiki **TACT** (Build Config / CDN Config field definitions; TACT product table: `wow` = WoW Retail).
- wowdev.wiki **CASC** (local storage layout: `data.NNN`, `.idx`, `config\XX\YY\<hash>`).
- TrinityCore support forum — a hand-made **8.3.7 (35662)** `.build.info` (the 14-column layout used in §4).
- Multiple real `.build.info` files (6.0.3 WoW; BfA 8.0.1; D2R; StarCraft) confirming the field set
  varies by version and that `Branch` is a region code while `Product` is the program code.
- This core's own source: `src/tools/map_extractor/System.cpp`, `extractor_common/CascHandles.cpp`
  (extractor uses CascLib directly and ignores `.build.info`).
