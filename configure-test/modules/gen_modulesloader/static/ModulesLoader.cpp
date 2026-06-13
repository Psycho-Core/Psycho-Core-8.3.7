// This file was created automatically from your module configuration!
// Use CMake to reconfigure this file, never change it on your own!

/* #undef TRINITY_IS_DYNAMIC_SCRIPTLOADER */

#include "Define.h"
#include <vector>
#include <string>

void Addmod_psychobotScripts();

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
#  include "ModulesLoader.h"
#  define TC_SCRIPT_API
#endif

/// Exposed in modules to register all of their scripts to the ScriptMgr.
TC_SCRIPT_API void AddModulesScripts()
{
    Addmod_psychobotScripts();
}

/// Exposed in script modules to get the build directive of the module.
TC_SCRIPT_API char const* GetBuildDirective()
{
    return _BUILD_DIRECTIVE;
}

#ifdef TRINITY_IS_DYNAMIC_SCRIPTLOADER
} // extern "C"
#endif
