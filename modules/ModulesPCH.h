/*
 * ===========================================================================
 *  modules/ModulesPCH.h - forced standard-include shim for the modules target
 *  Copyright (c) 2026 Psycho-core.
 * ===========================================================================
 *
 * WHY THIS EXISTS
 * --------------
 * The core (game/scripts/worldserver) is built with a precompiled header
 * (e.g. gamePCH.h) that pulls in Creature.h / Player.h / World.h, whose deep
 * include chains transitively make every std container available to *every*
 * core translation unit. Many core headers were therefore written to USE
 * std::unordered_map / std::map / std::string / std::vector / ... WITHOUT a
 * direct #include for them, relying on that transitive PCH availability.
 *
 * The modules target (mod-psychobot) is built WITHOUT a PCH, so when it
 * includes those core headers the standard containers are suddenly undefined
 * (e.g. "C2039: 'unordered_map': is not a member of 'std'", "C3646: unknown
 * override specifier", hundreds of cascading syntax errors).
 *
 * This header is force-included into every module translation unit via
 * /FI (MSVC) / -include (GCC,Clang) in modules/CMakeLists.txt, so each module
 * TU starts with the same standard-library availability the core PCH provides.
 * It fixes all current and future non-PCH standard-include errors in one place
 * without editing the upstream core headers.
 *
 * Keep this list to STANDARD LIBRARY headers only (no project headers), and
 * keep it in sync with whatever the core PCH transitively provides.
 * ===========================================================================
 */

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <set>
#include <shared_mutex>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
