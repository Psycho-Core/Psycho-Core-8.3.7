# Psycho_Core module build engine. Mirrors src/server/scripts/CMakeLists.txt
# but operates on the top-level modules/ directory (modules/mod-<name>).

message("")

# Make the module list available in the current scope
GetModuleSourceList(MODULES_MODULE_LIST)

# Make the native install offset available in this scope
GetInstallOffset(INSTALL_OFFSET)

# Sets MODULES_DEFAULT_LINKAGE from the MODULES build option
if (MODULES MATCHES "dynamic")
  set(MODULES_DEFAULT_LINKAGE "dynamic")
elseif(MODULES MATCHES "static")
  set(MODULES_DEFAULT_LINKAGE "static")
else()
  set(MODULES_DEFAULT_LINKAGE "disabled")
endif()

# Resolve each module's linkage (default -> follows MODULES) and build a graph.
foreach(SOURCE_MODULE ${MODULES_MODULE_LIST})
  ModuleNameToVariable(${SOURCE_MODULE} MODULE_MODULE_VARIABLE)
  if (NOT DEFINED ${MODULE_MODULE_VARIABLE})
    set(${MODULE_MODULE_VARIABLE} "default")
  endif()
  if (${MODULE_MODULE_VARIABLE} STREQUAL "default")
    set(${MODULE_MODULE_VARIABLE} ${MODULES_DEFAULT_LINKAGE})
  endif()

  if (${MODULE_MODULE_VARIABLE} MATCHES "dynamic")
    GetProjectNameOfModuleName(${SOURCE_MODULE} MODULE_PROJECT_NAME)
    GetNativeSharedLibraryName(${MODULE_PROJECT_NAME} MODULE_PROJECT_LIBRARY)
    list(APPEND MODULE_GRAPH_KEYS ${MODULE_PROJECT_NAME})
    set(MODULE_GRAPH_VALUE_DISPLAY_${MODULE_PROJECT_NAME} ${MODULE_PROJECT_LIBRARY})
    list(APPEND MODULE_GRAPH_VALUE_CONTAINS_${MODULE_PROJECT_NAME} ${SOURCE_MODULE})
  elseif(${MODULE_MODULE_VARIABLE} MATCHES "static")
    list(APPEND MODULE_GRAPH_KEYS worldserver)
    set(MODULE_GRAPH_VALUE_DISPLAY_worldserver worldserver)
    list(APPEND MODULE_GRAPH_VALUE_CONTAINS_worldserver ${SOURCE_MODULE})
  else()
    list(APPEND MODULE_GRAPH_KEYS disabled)
    set(MODULE_GRAPH_VALUE_DISPLAY_disabled disabled)
    list(APPEND MODULE_GRAPH_VALUE_CONTAINS_disabled ${SOURCE_MODULE})
  endif()
endforeach()

if (MODULE_GRAPH_KEYS)
  list(SORT MODULE_GRAPH_KEYS)
  list(REMOVE_DUPLICATES MODULE_GRAPH_KEYS)
endif()

# Display the module graph
message("* Module configuration (${MODULES}):
    |")
foreach(MODULE_GRAPH_KEY ${MODULE_GRAPH_KEYS})
  if (NOT MODULE_GRAPH_KEY STREQUAL "disabled")
    message("    +- ${MODULE_GRAPH_VALUE_DISPLAY_${MODULE_GRAPH_KEY}}")
  else()
    message("    |  ${MODULE_GRAPH_VALUE_DISPLAY_${MODULE_GRAPH_KEY}}")
  endif()
  foreach(MODULE_GRAPH_ENTRY ${MODULE_GRAPH_VALUE_CONTAINS_${MODULE_GRAPH_KEY}})
    message("    |   +- ${MODULE_GRAPH_ENTRY}")
  endforeach()
  message("    |")
endforeach()

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

# Configures the modules loader with the given name and stores the output in the
# LOADER_OUT variable. Mirrors ConfigureScriptLoader but uses the modules loader
# template and AddModulesScripts() entry point.
function(ConfigureModulesLoader LOADER_NAME LOADER_OUT IS_DYNAMIC_SCRIPTLOADER)
  set(TRINITY_IS_DYNAMIC_SCRIPTLOADER ${IS_DYNAMIC_SCRIPTLOADER})
  unset(TRINITY_SCRIPTS_FORWARD_DECL)
  unset(TRINITY_SCRIPTS_INVOKE)
  set(TRINITY_CURRENT_SCRIPT_PROJECT ${LOADER_NAME})
  foreach(LOCALE_MODULE ${ARGN})
    GetLoaderNameOfModuleName(${LOCALE_MODULE} LOADER_SUFFIX)
    set(LOADER_FUNCTION "Add${LOADER_SUFFIX}Scripts()")
    set(TRINITY_SCRIPTS_FORWARD_DECL
      "${TRINITY_SCRIPTS_FORWARD_DECL}void ${LOADER_FUNCTION};\n")
    set(TRINITY_SCRIPTS_INVOKE
      "${TRINITY_SCRIPTS_INVOKE}    ${LOADER_FUNCTION};\n")
  endforeach()
  set(GENERATED_LOADER ${CMAKE_CURRENT_BINARY_DIR}/gen_modulesloader/${LOADER_NAME}/ModulesLoader.cpp)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ModulesLoader.cpp.in.cmake ${GENERATED_LOADER})
  set(${LOADER_OUT} ${GENERATED_LOADER} PARENT_SCOPE)
endfunction()

# Build the per-module targets.
foreach(SOURCE_MODULE ${MODULES_MODULE_LIST})
  GetPathToModuleSource(${SOURCE_MODULE} MODULE_SOURCE_PATH)
  ModuleNameToVariable(${SOURCE_MODULE} MODULE_MODULE_VARIABLE)

  if ((${MODULE_MODULE_VARIABLE} STREQUAL "disabled") OR
      (${MODULE_MODULE_VARIABLE} STREQUAL "static"))
    GetProjectNameOfModuleName(${SOURCE_MODULE} MODULE_PROJECT_NAME)
    GetNativeSharedLibraryName(${MODULE_PROJECT_NAME} MODULE_OUTPUT_NAME)
    list(APPEND DISABLED_MODULE_PROJECTS ${INSTALL_OFFSET}/${MODULE_OUTPUT_NAME})
    if (${MODULE_MODULE_VARIABLE} STREQUAL "static")
      list(APPEND STATIC_MODULES ${SOURCE_MODULE})
      CollectSourceFiles(${MODULE_SOURCE_PATH} PRIVATE_SOURCES_MODULES)
      CollectIncludeDirectories(${MODULE_SOURCE_PATH} PUBLIC_INCLUDES_MODULES)
    endif()
  elseif(${MODULE_MODULE_VARIABLE} STREQUAL "dynamic")
    unset(MODULE_PRIVATE_SOURCES)
    CollectSourceFiles(${MODULE_SOURCE_PATH} MODULE_PRIVATE_SOURCES)
    CollectIncludeDirectories(${MODULE_SOURCE_PATH} PUBLIC_INCLUDES_MODULES)
    ConfigureModulesLoader(${SOURCE_MODULE} MODULE_PRIVATE_LOADER ON ${SOURCE_MODULE})
    GetProjectNameOfModuleName(${SOURCE_MODULE} MODULE_PROJECT_NAME)
    list(APPEND DYNAMIC_MODULE_PROJECTS ${MODULE_PROJECT_NAME})

    add_library(${MODULE_PROJECT_NAME} SHARED
      ${MODULE_PRIVATE_SOURCES}
      ${MODULE_PRIVATE_LOADER})

    target_link_libraries(${MODULE_PROJECT_NAME}
      PRIVATE
        trinity-core-interface
      PUBLIC
        game)

    target_include_directories(${MODULE_PROJECT_NAME}
      PUBLIC
        ${MODULE_SOURCE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR})

    set_target_properties(${MODULE_PROJECT_NAME}
      PROPERTIES
        FOLDER "modules")

    if (UNIX)
      install(TARGETS ${MODULE_PROJECT_NAME}
        DESTINATION ${INSTALL_OFFSET}
        COMPONENT ${MODULE_PROJECT_NAME})
    elseif (WIN32)
      install(TARGETS ${MODULE_PROJECT_NAME}
        RUNTIME DESTINATION ${INSTALL_OFFSET}
        COMPONENT ${MODULE_PROJECT_NAME})
      if (MSVC)
        set_target_properties(${MODULE_PROJECT_NAME} PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug/scripts
          RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release/scripts
          RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/scripts
          RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin/MinSizeRel/scripts)
      endif()
    endif()
  else()
    message(FATAL_ERROR "Unknown linkage value \"${${MODULE_MODULE_VARIABLE}}\" for module (${SOURCE_MODULE})!")
  endif()
endforeach()

# Expose dynamic module projects so worldserver can depend on them.
set(WORLDSERVER_DYNAMIC_MODULES_DEPENDENCIES ${DYNAMIC_MODULE_PROJECTS} PARENT_SCOPE)

# Generate the static modules loader (calls Add<Module>Scripts() for static mods)
ConfigureModulesLoader("static" MODULE_PRIVATE_LOADER OFF ${STATIC_MODULES})
if (MODULE_PRIVATE_LOADER)
  list(REMOVE_DUPLICATES MODULE_PRIVATE_LOADER)
endif()

# Build the single static "modules" library (always created, even if empty, so
# the scripts target / worldserver always have AddModulesScripts() available).
add_library(modules STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/ModulesLoader.h
  ${MODULE_PRIVATE_LOADER}
  ${PRIVATE_SOURCES_MODULES})

target_link_libraries(modules
  PRIVATE
    trinity-core-interface
  PUBLIC
    game-interface)

target_include_directories(modules
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PUBLIC_INCLUDES_MODULES})

set_target_properties(modules
  PROPERTIES
    FOLDER "modules")

# Allow each module to inject extra CMake (find_package/link/include) inline.
foreach(SOURCE_MODULE ${MODULES_MODULE_LIST})
  include("${CMAKE_SOURCE_DIR}/modules/${SOURCE_MODULE}/${SOURCE_MODULE}.cmake" OPTIONAL)
endforeach()

# Copy each enabled module's conf/*.conf.dist to <server root>/conf-backup/ (restore
# reference only; live module keys are merged into worldserver.conf for this core).
message(STATUS "* Module config files (-> conf-backup):")
foreach(SOURCE_MODULE ${MODULES_MODULE_LIST})
  ModuleNameToVariable(${SOURCE_MODULE} MODULE_MODULE_VARIABLE)
  if (NOT (${MODULE_MODULE_VARIABLE} STREQUAL "disabled"))
    GetPathToModuleConfig(${SOURCE_MODULE} MODULE_CONFIG_PATH)
    if (IS_DIRECTORY "${MODULE_CONFIG_PATH}")
      file(GLOB MODULE_CONFIG_LIST RELATIVE
        ${MODULE_CONFIG_PATH}
        ${MODULE_CONFIG_PATH}/*.conf.dist)
      foreach(configFileName ${MODULE_CONFIG_LIST})
        CopyModuleConfig("${MODULE_CONFIG_PATH}/${configFileName}")
        message(STATUS "    +- ${SOURCE_MODULE}: ${configFileName}")
      endforeach()
    endif()
  endif()
endforeach()

# Remove stale shared libraries in the install dir for disabled/static modules.
if (DISABLED_MODULE_PROJECTS)
  install(CODE "
    foreach(MODULE_TO_UNINSTALL ${DISABLED_MODULE_PROJECTS})
      if (EXISTS \"\${MODULE_TO_UNINSTALL}\")
        message(STATUS \"Uninstalling module: \${MODULE_TO_UNINSTALL}\")
        file(REMOVE \"\${MODULE_TO_UNINSTALL}\")
      endif()
    endforeach()
  ")
endif()

message("")
