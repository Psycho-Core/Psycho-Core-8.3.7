# How to Rebuild a Missing `.build.info` (WoW 8.3.7.35662)
**For:** Psycho-Core 8.3.7.35662 data extraction
**Goal:** Restore the `.build.info` file an extractor needs, when a client is missing it.

================================================================================
## READ THIS FIRST — does rebuilding even apply to you?
================================================================================
`.build.info` is a small TEXT manifest at the WoW client ROOT (next to Wow.exe). The
extractor reads it to learn the client BUILD, LOCALE, and which CASC config to load.

Rebuilding it ONLY works if the client's actual data is on disk. So check first:

WINDOWS (Command Prompt) — point these at your client folder:
```
dir  "PATH\TO\WoW-8.3.7\Data\data"
dir  "PATH\TO\WoW-8.3.7\Data\config"
```
- ✅ COMPLETE CLIENT: `Data\data\` has large `data.000`, `data.001`... files (many GB total),
  and `Data\config\` has nested hash sub-folders.  -> Rebuilding `.build.info` WILL help.
- ❌ STREAMING / STUB CLIENT (Firestorm / WoW Circle mini-clients): `Data\data\` is tiny/empty
  or `Data\config\` is sparse.  -> NO `.build.info` can fix this; the data isn't there.
  You need a COMPLETE 35662 client or a pre-extracted data pack instead.

If you're complete, continue. Two methods below: TOOL (easy) and MANUAL.

================================================================================
## WHAT A `.build.info` LOOKS LIKE (target file)
================================================================================
It is one header line + one data line, fields separated by `|`. Example shape
(values are placeholders — yours come from your client):
```
Branch!STRING:0|Active!DEC:1|Build Key!HEX:16|CDN Key!HEX:16|Install Key!HEX:16|IM Size!DEC:4|CDN Path!STRING:0|CDN Hosts!STRING:0|CDN Servers!STRING:0|Tags!STRING:0|Armadillo!STRING:0|Last Activated!STRING:0|Version!STRING:0
wow|1|<BUILDCONFIG_HASH>|<CDNCONFIG_HASH>|<INSTALL_HASH>|0|tpr/wow|level3.blizzard.com|http://level3.blizzard.com/?maxhosts=4|Windows x86_64 US? enUS speech:enUS text:enUS|||8.3.7.35662|
```
The 3 things that REALLY matter for local extraction:
  - Build Key   = the BuildConfig hash  (32 hex chars)
  - CDN Key     = the CDNConfig hash    (32 hex chars)
  - Tags        = must include your locale, e.g. `enUS`
  - Version     = 8.3.7.35662
Save the file as exactly `.build.info` (note the leading dot) at the client ROOT.

================================================================================
## METHOD A — USING A TOOL  (easiest, recommended)
================================================================================
Tools that read CASC can either rebuild `.build.info` for you, or show you the two
config hashes so you can paste them in.

### Option A1 — CASCExplorer / CascView  (GUI, shows the hashes)
1. Download CASCExplorer (a.k.a. CascView) — a free CASC viewer (Ladislav Zezula's CascLib GUI).
2. Run it -> "Open Storage" (local) -> browse to your client's `Data` folder (or the client root).
3. If it asks you to PICK A BUILD, choose 8.3.7.35662 / the matching BuildConfig.
4. Open its storage info / properties panel. It displays:
      - Build Config (BuildConfig) hash
      - CDN Config (CDNConfig) hash
      - Build name / version
      - Installed locale(s)
5. Copy those hashes. If CASCExplorer can open the storage, your data is good — now either:
      a) let any "build.info fixer" use them, or
      b) paste them into the manual template (Method B step 5).

### Option A2 — A `.build.info` "fixer" script
Some community scripts/tools scan `Data\config\` + `Data\indices\` and emit a valid
`.build.info` automatically. Steps are always:
1. Place the tool next to (or point it at) the client root.
2. Run it; it reads the config hashes from the client and writes `.build.info`.
3. Confirm a new `.build.info` appeared at the client root (next to Wow.exe).
4. Run the extractor — you should now see:
      `Opened casc storage '...\Data'`
      `Detected client build 35662 for locale enUS`

> TIP: whichever tool you use, the values it writes come FROM YOUR CLIENT — that's why
> a hand-made "generic" build.info from someone else won't work, but a tool-rebuilt one will.

================================================================================
## METHOD B — MANUAL REBUILD  (no special tool, just File Explorer + Notepad)
================================================================================
You pull the two hashes straight out of the client's own folders, then write the file.

### Step 1 — Show hidden files
Windows Explorer -> View -> tick "Hidden items" (so you can see/create `.build.info`).

### Step 2 — Find the BuildConfig + CDNConfig hashes
Open:  `PATH\TO\WoW-8.3.7\Data\config\`
You'll see a 2-level nested folder tree, e.g.:
```
Data\config\
  ├── 1e\
  │    └── 32\
  │         └── 1e32a7b9c4d5...   (32-hex-char file)  <-- one of these is BuildConfig
  ├── 4c\
  │    └── 82\
  │         └── 4c82f0a1...       (32-hex-char file)  <-- another is CDNConfig
```
Each bottom file's NAME is a config hash. You typically have a BuildConfig and a CDNConfig.

How to tell them apart (open each in Notepad — they're text):
  - BuildConfig contains lines like: `root = ...`, `install = ...`, `encoding = ...`,
    `build-name = WOW-35662patch8.3.7...`, `build-uid = wow`.   <-- this is the BUILD KEY
  - CDNConfig contains lines like: `archives = ...`, `archive-group = ...`,
    `patch-archives = ...`.                                     <-- this is the CDN KEY

Write down both 32-char hashes (the FILE NAMES, not the contents).

CMD shortcut to list them:
```
dir /s /b "PATH\TO\WoW-8.3.7\Data\config"
```
Then open the two files to identify which is BuildConfig vs CDNConfig.

### Step 3 — Confirm your locale
Open `PATH\TO\WoW-8.3.7\WTF\Config.wtf` and note:
```
SET textLocale "enUS"
SET audioLocale "enUS"
```
Use that locale (e.g. enUS) in the Tags field.

### Step 4 — (Install Key, optional) 
The Install Key is another config hash; for local extraction it's not critical. You can
reuse the BuildConfig hash or leave a 32-char hash from config if unsure — the extractor
mainly needs Build + CDN + locale. (If extraction complains, set it to the real install
hash, which BuildConfig references on its `install = <hash>` line.)

### Step 5 — Create `.build.info` at the client ROOT
In the WoW client root (next to Wow.exe), create a new text file, paste the template,
fill in YOUR hashes + locale + version, then save it as `.build.info`
(In Notepad: File > Save As > File name: `.build.info` , Save as type: All Files).

Template to fill (replace the <...> parts):
```
Branch!STRING:0|Active!DEC:1|Build Key!HEX:16|CDN Key!HEX:16|Install Key!HEX:16|IM Size!DEC:4|CDN Path!STRING:0|CDN Hosts!STRING:0|CDN Servers!STRING:0|Tags!STRING:0|Armadillo!STRING:0|Last Activated!STRING:0|Version!STRING:0
wow|1|<BUILDCONFIG_HASH>|<CDNCONFIG_HASH>|<INSTALL_HASH>|0|tpr/wow|level3.blizzard.com|http://level3.blizzard.com/?maxhosts=4|Windows x86_64 US? enUS speech:enUS text:enUS|||8.3.7.35662|
```
- `<BUILDCONFIG_HASH>` = the BuildConfig file name from Step 2
- `<CDNCONFIG_HASH>`   = the CDNConfig file name from Step 2
- `<INSTALL_HASH>`     = install hash (or reuse BuildConfig hash if unknown)
- locale in Tags = your locale from Step 3
- Version = 8.3.7.35662

### Step 6 — Test
`cd` INTO the client folder and run the extractor by full path, e.g.:
```
cd  "PATH\TO\WoW-8.3.7"
"PATH\TO\server\bin\mapextractor.exe"
```
SUCCESS looks like:
```
Opened casc storage '...\Data'
Detected client build 35662 for locale enUS
```
If you still get "No locales detected / FILE_NOT_FOUND":
  - the hashes are wrong (re-check which file is BuildConfig vs CDNConfig), OR
  - the locale tag doesn't match the data present, OR
  - (most common) the client's Data\ is INCOMPLETE -> rebuild can't help; get a full client.

================================================================================
## QUICK SUMMARY
================================================================================
1. Verify Data\data\ is multi-GB and Data\config\ has nested hash folders (else stop — stub client).
2. TOOL way: CASCExplorer opens the Data folder and shows BuildConfig + CDNConfig hashes.
3. MANUAL way: read the two 32-char hashes from Data\config\ (BuildConfig has root/encoding/
   build-name; CDNConfig has archives/archive-group), grab locale from WTF\Config.wtf.
4. Write `.build.info` at the client root using the template, filling in those hashes + enUS + 8.3.7.35662.
5. Run mapextractor from inside the client folder; expect "Detected client build 35662".

NOTE: `.build.info` is per-client. The values must come from THAT client's own Data\config\.
A generic/borrowed build.info will fail unless its data blobs match — which is why we read
the hashes from your client rather than guessing.
