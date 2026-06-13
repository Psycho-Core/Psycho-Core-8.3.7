#ifndef PSYCHO_MODULES_LOADER_H
#define PSYCHO_MODULES_LOADER_H

#include "Define.h"

// Registers all statically-linked modules with the ScriptMgr.
// Called once from the static script loader (see src/server/scripts).
void AddModulesScripts();

#endif // PSYCHO_MODULES_LOADER_H
