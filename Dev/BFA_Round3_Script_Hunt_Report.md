# BFA Core — Round 3 Script Hunt & Verification Report
**Date:** 2026-06-14
**Your base verified:** `Psycho-Core-8.3.7` (AzgathCore/Titans lineage, build 35662)
**Method:** Directly cloned candidate sources (git tree only) and diffed every script
directory against your actual working tree — no reliance on prior audit assumptions.

---

## PART A — VERIFICATION OF YOUR EXISTING SCRIPTS

Your `src/server/scripts/` tree was re-counted file-by-file. **The Master Tree audit
was accurate for dungeon/raid directories** — every leaf directory count matched
exactly. However, two earlier-audit claims are **WRONG** and are corrected here:

### ❌ Correction 1 — There is NO `Spells/` directory in your core
The prior audits all assumed a `scripts/Spells/` directory with ~20 `.cpp` files.
**It does not exist in your repo.** You have only 2 stray `spell_*.cpp` files buried in
dungeon folders (Bloodmaul, Skyreach). This is your **single largest real gap** — the
entire class-spell script layer is absent.

### ✅ Correction 2 — Your zone/open-world coverage is MUCH better than audited
The earlier "Found/Missing" docs claimed Argus, Suramar, Mardum, BFA open-world zones,
etc. were missing or stubs. **False.** Your repo actually contains **96 `zone_*.cpp`
scripts**, including:
- Argus: `zone_argus.cpp`, `zone_argus_krokuun.cpp`, `zone_argus_macaree.cpp`, `zone_argus_antoran_wastes.cpp`
- BFA: `zone_drustvar`, `zone_zuldazar`, `zone_nazmir`, `zone_voldun`, `zone_tiragarde_sound`, `zone_stormsong_valley`, `zone_nazjatar`, `zone_arathi_highlands`, `zone_darkshore`
- Legion: `zone_suramar`, `zone_mardum`, `zone_vault_of_wardens`, `zone_azsuna`, `zone_valsharah`, `zone_stormheim`, `zone_highmountain`, `zone_broken_shore`, `zone_dalaran_legion`
- Plus essentially every Classic→Legion leveling zone.

### Headline number
**Your core has 1,318 `.cpp` script files — the MOST of any repository examined this round.**

| Repo | Script `.cpp` files | Has `Spells/`? |
|---|---|---|
| **Psycho-Core-8.3.7 (yours)** | **1,318** | ❌ **NO** |
| LoA-BFA (zTerragor) | 1,313 | ✅ yes |
| DekkCore Shadowlands | 1,246 (incl SL) | ✅ yes (13) |
| AshamaneCore (Legion) | 1,125 | ✅ yes (21) |
| Official TrinityCore 8.3.7 | 658 | ✅ yes (18) |

---

## PART B — REPOSITORIES & TAGS HUNTED THIS ROUND

| Source | Type | Result |
|---|---|---|
| **TrinityCore git tags** | Official | Only **TDB-prefixed DB tags**. Two are BFA 8.3.7: `TDB837.20081`, `TDB837.20101`. Branches are only `3.3.5`, `cata_classic`, `wotlk_classic`, `master`. **No BFA code branch remains upstream** — the 837 tags are the last 8.3.7 code. |
| **TrinityCore @ TDB837.20101** | Official 8.3.7 | ✅ Clean upstream source. Full `Spells/` suite + `Argus/`. Fewer boss dirs than yours (it's the vanilla TC base your fork expanded on). |
| **mengjingxuan/TrinityCore-Legion** (AshamaneCore mirror) | Legion 7.x | ✅ Full `Spells/` (21 files incl `spell_artifact`, `spell_toy`), large Legion scripts. |
| **zTerragor/Legends-of-Azeroth-BFA** | BFA 35662 | Nearly identical tree to yours — **its one advantage is the `Spells/` directory.** |
| **devovh/Shadowlands_Dekk-Core** | SL 9.x (w/ BFA+Legion) | ✅ Richest. Has the **only public Crucible of Storms**, BFA world bosses, Ashran, 31 scenarios. |

---

## PART C — WHAT YOU CAN ACTUALLY ADD (ranked by value)

### 🔴 TIER 1 — The big one: the class-spell script layer
**Source (best): Official TrinityCore `TDB837.20101` → `src/server/scripts/Spells/`**
- 18 files: `spell_dh, dk, druid, generic, holiday, hunter, item, mage, monk, paladin, pet, priest, quest, rogue, shaman, warlock, warrior, spell_script_loader`.
- This is the **authoritative, build-matched** version — same 8.3.7 API as your fork, lowest merge risk.
- Alternative (more content, more risk): **AshamaneCore** versions are larger (e.g. `spell_warlock.cpp` ~245 KB) and add `spell_artifact.cpp`, `spell_mastery.cpp`, `spell_toy.cpp` — but they're Legion-era and may need adaptation.
- **Recommendation:** Take TC 837's `Spells/` as the clean base, then optionally cherry-pick Ashamane's `spell_artifact` / `spell_toy` / `spell_mastery` on top.

### 🔴 TIER 2 — Crucible of Storms raid (still the only public copy)
**Source: DekkCore** → `DekkCore/BattleforAzeroth/CrucibleOfStorms/`
- `boss_restless_cabal.cpp`, `boss_uunat.cpp`, `instance_crucible_of_storms.cpp`, `crucible_of_storms.h`
- Confirmed present; nowhere else. Merge difficulty MEDIUM (DekkCore subfolder + CMake/script-name adaptation).

### 🟡 TIER 3 — Smaller fills from DekkCore (verify they beat yours before merging)
- **Ashran** (WoD BG): `DekkCore/Draenor/Ashran/` (`AshranQuest.cpp`, `AshranDatas.h`) — you have none.
- **BFA World Bosses**: `boss_dunegorger_kraulok` (Kul Tiras), `boss_jiarak`, `boss_tzane` — you only have 3 Zandalar world bosses; Kul Tiras side is thin.
- **Extra MoP/WoD scenarios** (31 total in Dekk) — but many overlap what you have.

### 🟢 TIER 4 — Marginal / mostly renames (low value)
- AshamaneCore's "extra" dirs (`Anthorus`, `CathedralOfEternalNight`, `KarazhanLegion`) are **renamed equivalents** of dirs you already have (`AntorusTheBurningThrone` 14 files, `CathedralEternalNight`, `ReturnToKharazan`). Only worth diffing individual bosses if you want more-complete versions.
- Ashamane has slightly more in `TrialOfValor` (7 vs your 5) and `Gilneas` (4 vs your 1) — minor.
- LoA-BFA `BattleOfDazarAlor` 12 vs your 11 — one extra file, marginal.

---

## PART D — STILL MISSING EVERYWHERE (no public source exists — confirmed again)
These were never written publicly and remain custom-dev-only:
- Warfronts (Arathi / Darkshore) — *systems*; you do have empty `zone_arathi_highlands.cpp`/`zone_darkshore.cpp` shells
- Island Expeditions (full AI/rotation/rewards)
- Heart of Azeroth / Azerite traits, Corruption system
- Horrific Visions, N'Zoth Assaults (Vale/Uldum)
- War Mode, Mercenary Mode, PvP Brawls
- BGs: Seething Shore, Silvershard Mines, Temple of Kotmogu, Deepwind Gorge
- Artifact Challenges (Mage Tower), Netherlight Crucible, Proving Grounds, Class Hall missions, Withered Army Training, Deaths of Chromie

---

## BOTTOM LINE
1. **Your scripts verified** — your core is actually the most complete of the bunch (1,318 cpp) and your zone coverage is far better than the old audit claimed.
2. **One genuine, high-impact gap:** the missing `scripts/Spells/` class-spell layer → **pull from official TrinityCore `TDB837.20101`** (perfect build match).
3. **One genuine content addition:** **Crucible of Storms** from DekkCore (still unique).
4. **Everything else** is either renames, marginal single-file diffs, or content that simply does not exist publicly.
