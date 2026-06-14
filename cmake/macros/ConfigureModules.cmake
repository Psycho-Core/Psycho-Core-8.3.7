# Psycho_Core module system - mirrors the script-module helpers in
# ConfigureScripts.cmake but targets the top-level "modules/" directory
# (AzerothCore-style modules/mod-<name> layout).

# Returns the base path to the modules directory in the source directory
function(GetModulesBasePath variable)
  set(${variable} "${CMAKE_SOURCE_DIR}/modules" PARENT_SCOPE)
endfunction()

# Stores the absolute path to the given module's src/ folder in the variable
function(GetPathToModuleSource module variable)
  GetModulesBasePath(MODULE_BASE_PATH)
  set(${variable} "${MODULE_BASE_PATH}/${module}/src" PARENT_SCOPE)
endfunction()

# Stores the absolute path to the given module's conf/ folder in the variable
function(GetPathToModuleConfig module variable)
  GetModulesBasePath(MODULE_BASE_PATH)
  set(${variable} "${MODULE_BASE_PATH}/${module}/conf" PARENT_SCOPE)
endfunction()

# Stores the project (target) name of the given module in the variable
# e.g. "mod-skeleton" -> "module_mod_skeleton"
function(GetProjectNameOfModuleName module variable)
  string(REGEX REPLACE "[-]" "_" SANITIZED "${module}")
  string(TOLOWER "module_${SANITIZED}" GENERATED_NAME)
  set(${variable} "${GENERATED_NAME}" PARENT_SCOPE)
endfunction()

# Stores the script-loader function suffix for the given module in the variable.
# The generated loader expects  void Add<Suffix>Scripts();
# Hyphens are stripped so "mod-skeleton" -> loader "Addmod_skeletonScripts".
function(GetLoaderNameOfModuleName module variable)
  string(REGEX REPLACE "[-]" "_" SANITIZED "${module}")
  set(${variable} "${SANITIZED}" PARENT_SCOPE)
endfunction()

# Creates a list of all modules (directories under modules/ that contain a src/)
# and stores it in the given variable.
function(GetModuleSourceList variable)
  GetModulesBasePath(BASE_PATH)
  if (NOT IS_DIRECTORY "${BASE_PATH}")
    set(${variable} "" PARENT_SCOPE)
    return()
  endif()

  file(GLOB LOCALE_MODULE_LIST RELATIVE
    ${BASE_PATH}
    ${BASE_PATH}/*)

  set(${variable})
  foreach(SOURCE_MODULE ${LOCALE_MODULE_LIST})
    GetPathToModuleSource(${SOURCE_MODULE} MODULE_SOURCE_PATH)
    # Only treat as a module if it has a src/ directory (skips README, etc.)
    if (IS_DIRECTORY ${MODULE_SOURCE_PATH})
      list(APPEND ${variable} ${SOURCE_MODULE})
    endif()
  endforeach()
  set(${variable} ${${variable}} PARENT_SCOPE)
endfunction()

# Converts the given module name into its linkage-type cache variable name.
# e.g. "mod-skeleton" -> "MODULE_MOD_SKELETON"
function(ModuleNameToVariable module variable)
  string(REGEX REPLACE "[-]" "_" SANITIZED "${module}")
  string(TOUPPER ${SANITIZED} SANITIZED)
  set(${variable} "MODULE_${SANITIZED}" PARENT_SCOPE)
endfunction()

# Stores in the given variable whether any module needs dynamic linking
# (so the parent build can force BUILD_SHARED_LIBS, just like scripts do).
function(IsDynamicLinkingModulesRequired variable)
  set(IS_DEFAULT_VALUE_DYNAMIC OFF)
  if (MODULES MATCHES "dynamic")
    set(IS_DEFAULT_VALUE_DYNAMIC ON)
  endif()

  GetModuleSourceList(MODULES_MODULE_LIST)
  set(IS_REQUIRED OFF)
  foreach(SOURCE_MODULE ${MODULES_MODULE_LIST})
    ModuleNameToVariable(${SOURCE_MODULE} MODULE_MODULE_VARIABLE)
    if ((${MODULE_MODULE_VARIABLE} STREQUAL "dynamic") OR
        (${MODULE_MODULE_VARIABLE} STREQUAL "default" AND IS_DEFAULT_VALUE_DYNAMIC))
      set(IS_REQUIRED ON)
      break()
    endif()
  endforeach()
  set(${variable} ${IS_REQUIRED} PARENT_SCOPE)
endfunction()

# Copies a module's <name>.conf.dist into a BACKUP/restore location.
# Psycho_Core layout: the live module keys are MERGED into worldserver.conf (this core's
# ConfigMgr only loads worldserver.conf), so the module's own .conf.dist is shipped to:
#   <server root>/conf-backup/mod_psychobot.conf.dist
# as a pristine reference users can restore from if they break their worldserver.conf.
function(CopyModuleConfig configFilePath)
  if (WIN32)
    add_custom_command(TARGET modules POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/conf-backup
      COMMAND ${CMAKE_COMMAND} -E copy "${configFilePath}" ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/conf-backup/)
  else()
    add_custom_command(TARGET modules POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/conf-backup
      COMMAND ${CMAKE_COMMAND} -E copy "${configFilePath}" ${CMAKE_BINARY_DIR}/bin/conf-backup/)
  endif()

  if (UNIX)
    install(FILES "${configFilePath}" DESTINATION "${CONF_DIR}/../conf-backup")
  elseif (WIN32)
    install(FILES "${configFilePath}" DESTINATION "${CMAKE_INSTALL_PREFIX}/conf-backup")
  endif()
endfunction()
