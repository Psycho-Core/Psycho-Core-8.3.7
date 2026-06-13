/*
 * Copyright (C) 2022 BfaCore Reforged
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// This file was created automatically from your script configuration!
// Use CMake to reconfigure this file, never change it on your own!

/* #undef TRINITY_IS_DYNAMIC_SCRIPTLOADER */

#include "Define.h"
#include <vector>
#include <string>

void AddAlliedRacesScripts();
void AddBrawlersGuildScripts();
void AddBrokenIslesScripts();
void AddCommandsScripts();
void AddCustomScripts();
void AddDarkmoonIslandScripts();
void AddDraenorScripts();
void AddEasternKingdomsScripts();
void AddEventsScripts();
void AddKalimdorScripts();
void AddKulTirasScripts();
void AddMaelstromScripts();
void AddNazjatarScripts();
void AddNorthrendScripts();
void AddNyalothaScripts();
void AddOutdoorPvPScripts();
void AddOutlandScripts();
void AddPandariaScripts();
void AddPetScripts();
void AddScenariosScripts();
void AddSpellsScripts();
void AddWorldScripts();
void AddZandalarScripts();

#ifdef TRINITY_IS_DYNAMIC_SCRIPTLOADER
#  include "revision_data.h"
#  define TC_SCRIPT_API TC_API_EXPORT
extern "C" {

/// Exposed in script modules to return the script module revision hash.
TC_SCRIPT_API char const* GetScriptModuleRevisionHash()
{
    return _HASH;
}

/// Exposed in script module to return the name of the script module
/// contained in this shared library.
TC_SCRIPT_API char const* GetScriptModule()
{
    return "static";
}

#else
#  include "ScriptLoader.h"
#  define TC_SCRIPT_API
#endif

/// Exposed in script modules to register all scripts to the ScriptMgr.
TC_SCRIPT_API void AddScripts()
{
    AddAlliedRacesScripts();
    AddBrawlersGuildScripts();
    AddBrokenIslesScripts();
    AddCommandsScripts();
    AddCustomScripts();
    AddDarkmoonIslandScripts();
    AddDraenorScripts();
    AddEasternKingdomsScripts();
    AddEventsScripts();
    AddKalimdorScripts();
    AddKulTirasScripts();
    AddMaelstromScripts();
    AddNazjatarScripts();
    AddNorthrendScripts();
    AddNyalothaScripts();
    AddOutdoorPvPScripts();
    AddOutlandScripts();
    AddPandariaScripts();
    AddPetScripts();
    AddScenariosScripts();
    AddSpellsScripts();
    AddWorldScripts();
    AddZandalarScripts();
}

/// Exposed in script modules to get the build directive of the module.
TC_SCRIPT_API char const* GetBuildDirective()
{
    return _BUILD_DIRECTIVE;
}

#ifdef TRINITY_IS_DYNAMIC_SCRIPTLOADER
} // extern "C"
#endif
