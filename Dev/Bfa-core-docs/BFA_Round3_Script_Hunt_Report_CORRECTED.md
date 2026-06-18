# BFA Core — Round 3 Script Hunt (CORRECTED) + Full Verification
**Date:** 2026-06-14
**Your core:** `Psycho-Core-8.3.7` — verified complete, HEAD `8f0701c` == `origin/main`
**Method:** Fresh git-authoritative diff of your repo vs 4 freshly-cloned sources + TrinityCore tags.

> ⚠️ This report SUPERSEDES the earlier `BFA_Round3_Script_Hunt_Report.md`, which was
> based on an incomplete filesystem state and wrongly claimed your `Spells/` directory
> was missing. **Your `Spells/` directory exists and is complete (20 substantial files).**

---

## STEP 1 — FULL CORE VERIFICATION ✅
- Local HEAD `8f0701cda...` **exactly matches** GitHub `origin/main`.
- **5,406 tracked files**, working tree clean (0 modified, 0 untracked).
- 524 MB total: `src` 64M, `dep` 11M, `modules` 5.7M (incl. `mod-psychobot`), `sql` 1.1M.
- `git fsck --full --strict`: clean. **I have your entire, unmodified core.**

## STEP 2 — SCRIPT VERIFICATION ✅ (git-authoritative)
- **Total script `.cpp`: 1,338** — the most of any repo compared.
- **`Spells/`: 20 files, all real** (spell_generic 290 KB, spell_warlock 206 KB, spell_item 179 KB, spell_druid 153 KB, … + spell_artifact, spell_mastery, spell_toy, spell_pet, spell_quest). Properly registered via `spell_script_loader.cpp`.
- **`zone_*.cpp`: 95 scripts** — full Classic→BFA coverage incl. all Argus zones, Suramar, Mardum, Vault of Wardens, and BFA open-world (Drustvar, Zuldazar, Nazmir, Vol'dun, Nazjatar, Arathi, Darkshore, etc.).
- Top areas: EasternKingdoms 237, BrokenIsles 188, Northrend 164, Kalimdor 162, Pandaria 106, Outland 99, Draenor 96, Zandalar 57, KulTiras 45.

### Comparison baseline
| Repo | Script .cpp | Has `Spells/` |
|---|---|---|
| **Psycho-Core (yours)** | **1,338** | ✅ 20 (incl. artifact/mastery/toy) |
| LoA-BFA | 1,313 | ✅ 20 |
| DekkCore (incl. Shadowlands) | 1,246 | ✅ 13 (DekkCore) |
| AshamaneCore | 1,125 | ✅ 21 |
| Official TrinityCore 8.3.7 | 658 | ✅ 18 |

---

## STEP 3 — THE HUNT: what each source has that you DON'T

### vs Official TrinityCore `TDB837.20101`
TC's tags are **DB snapshots**; the two BFA ones are `TDB837.20081` and `TDB837.20101`. No BFA *branch* remains upstream (only 3.3.5 / cata_classic / wotlk_classic / master).
- **Nothing useful to add.** TC has *fewer* scripts than you everywhere. Only "difference" is it nests Antorus under `Argus/` (you have it as `BrokenIsles/AntorusTheBurningThrone`, 14 files — same content).

### vs AshamaneCore (Legion)
- "Extra" dirs `Anthorus`, `CathedralOfEternalNight`, `KarazhanLegion` = **renames** of your existing `AntorusTheBurningThrone` / `CathedralEternalNight` / `ReturnToKharazan`.
- Genuinely more files: **TrialOfValor 7 vs your 5**, **Gilneas 4 vs your 1**, **Spells +1 (`spell_holiday.cpp`)**. → Low-value, worth a per-file diff only if you want marginally more.

### vs LoA-BFA
- **Effectively identical to your core.** Only diffs: `Custom` 6 vs 5, `BattleOfDazarAlor` 12 vs 11 (one extra file each). Negligible.

### vs DekkCore (Shadowlands core — richest source) ⭐
This is where the real finds are:

| Find | Detail | You have | Value |
|---|---|---|---|
| **Crucible of Storms** | `boss_restless_cabal`, `boss_uunat`, `instance_crucible_of_storms` (+ .h) | ❌ 0 (and 0 in ALL other repos) | 🔴 **HIGH — only public copy** |
| **Ashran** (WoD BG) | `AshranQuest.cpp` + `AshranDatas.h` | ❌ 0 | 🟡 medium |
| **BFA World Bosses** | `boss_dunegorger_kraulok` (Kul Tiras), `boss_jiarak`, `boss_tzane` | partial (3 Zandalar only) | 🟡 medium |
| **Extra MoP/WoD scenarios** | ABrewingStorm, ArenaOfAnnihiliation, TrovesOfThunderKing, BrokenIslands, Gorgrond/Frostfire/Talador finales, Grommashar, etc. | you have 8 scenarios, mostly different | 🟡 medium |
| **Misc encounter extras** | `boss_galion` (Pandaria), `zone_goldshire`/`zone_chapter1` (EK), +1 file in ICC/Naxx/Ulduar/Ahnkahet/DraktharonKeep/UtgardePinnacle/CullingOfStratholme | mostly you ≈ equal | 🟢 low (verify per-file) |
| Shadowlands content | 90 cpp (Castle Nathria, Torghast, all SL dungeons) | ❌ | ⚪ **out of scope** (9.x, not 8.3.7) |

---

## STILL MISSING EVERYWHERE (re-confirmed — no public source)
Warfronts (Arathi/Darkshore systems), Island Expeditions (AI/rotation/rewards), Heart of Azeroth/Azerite traits, Corruption, Horrific Visions, N'Zoth Assaults (Vale/Uldum), War Mode, Mercenary Mode, PvP Brawls, BGs (Seething Shore, Silvershard Mines, Temple of Kotmogu, Deepwind Gorge), Mage Tower, Netherlight Crucible, Proving Grounds, Class Hall missions, Withered Army Training, Deaths of Chromie.

---

## BOTTOM LINE (corrected)
1. **I have your full, intact core** (matches GitHub exactly) and your scripts are verified: **1,338 cpp, incl. a complete `Spells/` layer and 95 zone scripts.** Your core is the most complete of everything compared.
2. **You do NOT need a `Spells/` import** — earlier advice retracted; yours equals or beats every source (only Ashamane's `spell_holiday.cpp` is a trivial extra).
3. **The one genuinely valuable add: Crucible of Storms** from DekkCore — still the only public copy of that raid.
4. **Minor optional adds from DekkCore:** Ashran, Kul Tiras world bosses, a few extra scenarios/bosses (`boss_galion`, `zone_goldshire`).
5. **TrinityCore tags** offer nothing beyond your core (they're DB snapshots; the code is a smaller upstream base you already exceed).
6. Everything else on the "missing" list does not exist publicly — custom-dev only.
