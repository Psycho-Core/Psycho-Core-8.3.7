# GitHub Release — copy/paste fields

**Tag:** `V1.0-Boost`
**Title:** `Pre-compiled Boost + EXE`

---

## Description (paste this into the release body)

Pre-compiled **Boost 1.83.0** for building Psycho-Core 8.3.7.35662.

These are dependency binaries only — not the server. Grab the one you need, extract it into `dep/boost\` in the source tree, then build. CMake **cannot read inside a zip**, so you must extract before configuring.

**Verified build:** Boost **1.83.0** · compiler **vc143 (Visual Studio 2022 / MSVC 14.3)** · **x64** · static libs (`mt-x64`). Static libs are baked into the `.exe` at link time, so no Boost DLLs are needed to run the server.

### Files in this release
| File | Size | Use it for |
|------|------|-----------|
| **boost_dep_release.zip** | ~26 MB | **Most users.** Release-only pack: `boost\` headers + `stage\lib\` (the 8 release libs: system, filesystem, thread, program_options, iostreams, regex, chrono, atomic). For normal Release / RelWithDebInfo builds. *(Also ships inside the repo at `dep/boost\`.)* |
| **boost_dep.zip** | ~280 MB | **Debug builds only.** Full Release + Debug pack (`lib64-msvc-14.3\` incl. `-gd-` debug libs + all components). |
| **boost_1_83_0-msvc-14.3-64.exe** | — | **Fallback / official.** Unmodified upstream Boost 1.83.0 installer for VS2022 x64. Use if a zip is corrupt or you want stock Boost. |

### How to install
1. Download the file that matches your build (Release → `boost_dep_release.zip`; Debug → `boost_dep.zip`; or run the `.exe`).
2. Extract it **in place** into `dep/boost\` so you get:
   - `dep/boost/boost/version.hpp` (headers), and
   - `dep/boost/stage/lib/` (release pack) **or** `dep/boost/lib64-msvc-14.3/` (debug pack).
3. **Nesting check:** if you see `dep/boost/boost_1_83_0/boost/...` you extracted one level too deep — move the inner folders up so `boost\` sits directly inside `dep/boost\`.
4. Build (see `Dev/BUILD_GUIDE_WINDOWS.md`).

### Trouble?
- CMake says it can't find Boost, or a zip looks incomplete → **re-download** (a half-finished download is the #1 cause), or use the `.exe`, or build from source.
- Full instructions + the from-source `b2` build commands: `dep/boost/INSTALL_BOOST.txt`.

---

## Upload reminder (for the maintainer)
- Attach all three files to the **"Attach binaries"** box at the BOTTOM of the *Draft a new release* page — NOT the description text box (that box has a 25 MB limit; the attach box allows up to 2 GB).
- If a 280 MB upload times out, publish the release first, then **Edit release** and drag the file into the assets box again.
