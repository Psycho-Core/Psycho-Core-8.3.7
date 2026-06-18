# Can the Server Read UN-Extracted Client Files Directly?
**Date:** 2026-06-14
**Question:** Instead of extracting maps/vmaps/mmaps, can Psycho-Core (TrinityCore-based, BFA 8.3.7)
read the raw, un-extracted WoW client `Data/` (CASC) directly?

---

## SHORT ANSWER: No — not with a stock TrinityCore-based core. It's not supported, and building it
yourself would be a major engineering project (months), not a config tweak.

> "For an emulator (such as TrinityCore) to function correctly, it needs to understand the World of
> Warcraft world exactly as the game client sees it. **However, the server cannot directly read the
> game files (CASC files). Extraction tools act as 'translators'** to convert visual data from the
> game into logical data that the server can process." — community tool documentation

The `worldserver` only knows how to load the **processed** outputs (`dbc/db2`, `maps`, `vmaps`,
`mmaps`) from `DataDir`. There is **no config option** to point it at a raw client `Data/` folder.
If the processed files aren't there, it refuses to boot:
```
VMap file './vmaps/0530.vmtree' does not exist ...
Unable to load critical files - server shutting down !!!
```

---

## WHY it works this way (this is the important part)

The client files are **the wrong format for what the server needs** — extraction isn't just
"unzipping," it's a *transformation*. The server cannot use raw client assets even if it could open
the CASC container:

| Output | Raw client has… | …but the SERVER needs | Extraction does |
|--------|-----------------|------------------------|-----------------|
| `maps/` | ADT terrain tiles (for rendering) | a stripped height-grid (`.map` v-numbered format) | re-encodes terrain into the server's own map format |
| `vmaps/` | WMO/M2 visual 3D models | collision/line-of-sight geometry trees (`.vmtree`/`.vmtile`) | **converts visual meshes → collision meshes** |
| `mmaps/` | **nothing** — not in the client at all | navigation meshes for pathfinding | **GENERATES** them with Recast/Detour from the vmaps |
| `dbc/db2` | CASC-packed DB2 tables | loose DB2 files in a known layout | unpacks + lays out per-locale |

Two killer points:
1. **`mmaps` don't exist in the client.** They are *computed* (Recast navmesh generation) from the
   collision data — there is literally nothing to "read directly." The server would have to run the
   navmesh generator at startup, which is the slow part (hours).
2. **The server uses its own versioned `.map` format.** It even rejects maps from a different
   build/format version ("incompatible map version, vX.Y is expected. Please recreate using the
   mapextractor"). So it's deeply tied to the extracted format, not the client format.

Additionally, the server is a **headless, cross-platform daemon** with no rendering/model code. The
CASC reader + WMO/M2 parsers + Recast generator live entirely in the *tools*, not in `worldserver`,
specifically so the runtime stays lean (a few GB of processed data vs. ~tens of GB raw client + heavy
parsing every boot).

---

## "But could it be made to?" — Theoretically yes, practically no

You *could* fork the core to embed the CASC reader and run extraction/generation in-process at
startup. But that means:
- Porting the entire `src/tools/` pipeline (CASC, map_extractor, vmap4_extractor/assembler,
  **mmaps_generator**) into the worldserver and running it on boot.
- The first boot would still take **hours** (mmap navmesh generation is unavoidable compute).
- You'd be maintaining a heavily-customized core. **No public BFA core does this.** It defeats the
  entire reason the split exists.

So "make the server read un-extracted files" = re-implement extraction inside the server. That's not
a shortcut; it's strictly more work than just running the extractor once.

---

## What this means for YOUR actual problem

Your blocker isn't "the server can't read the client" — it's that **the extractor tools are failing**
(can't find the data). The split architecture means extraction is **mandatory and unavoidable**, so
the productive paths are:

1. **Fix the extractor** (most likely a missing `.build.info` and/or running from the wrong
   directory — see `SHARED_CLIENT_DATA_SETUP_REPORT.md`). Extract once.
2. **Use a pre-extracted 8.3.7 (35662) data pack** — skips extraction entirely. Build-specific but
   NOT server-specific, so it drops straight into Psycho-Core via `DataDir`. This is the closest
   thing to "not having to extract" that actually works.
3. Park whichever you choose on a shared `DataDir` (the setup from the previous report) so you never
   redo it.

> Note: `extractor.bat` (in `contrib/`) is the standard one-shot way to run all extractors in order
> — several people who thought "the maps aren't there" actually just hadn't extracted properly, and
> running `extractor.bat` fixed it.

---

## BOTTOM LINE
- **Direct un-extracted reading: not possible** on a stock TrinityCore/Psycho-Core, and building it
  would be a huge custom project that's *slower*, not faster.
- **`mmaps` aren't even in the client** — they must be generated, so there's no "raw file" to read.
- **Your real fix:** make extraction succeed ONCE (fix `.build.info` / run from client dir) **or**
  grab a **pre-extracted 35662 data pack**, then serve it from a shared `DataDir`. That gets you the
  "set it up once, never touch it again" outcome you're after.
