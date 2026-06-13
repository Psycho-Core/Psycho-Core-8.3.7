# ===========================================================================
# Psycho_Core local FindBoost.cmake
# ===========================================================================
# CMake 4.x no longer ships the old FindBoost module. This project still uses
# classic Boost variables in dep/boost/CMakeLists.txt, so provide a small local
# finder that supports the normal TrinityCore requirements:
#   system filesystem thread program_options iostreams regex
#
# Supported Windows layouts include:
#   BOOST_ROOT=C:/local/boost_1_83_0
#   Boost_ROOT=C:/local/boost_1_83_0
#   C:/local/boost_1_83_0
#
# Expected Windows library folders include:
#   lib64-msvc-14.3, lib64-msvc-14.2, lib64-msvc-14.1, stage/lib, lib
# ===========================================================================

set(_BOOST_CANDIDATE_ROOTS
  "${Boost_ROOT}"
  "${BOOST_ROOT}"
  "${BOOSTROOT}"
  "$ENV{Boost_ROOT}"
  "$ENV{BOOST_ROOT}"
  "$ENV{BOOSTROOT}"
  "${CMAKE_SOURCE_DIR}/dep/boost"
  "C:/local/boost_1_83_0"
  "C:/local/boost_1_84_0"
  "C:/local/boost_1_85_0"
  "C:/local/boost_1_86_0"
  "C:/Boost"
)

# Remove empty entries to avoid noisy searching.
set(_BOOST_ROOTS "")
foreach(_boost_root ${_BOOST_CANDIDATE_ROOTS})
  if(_boost_root)
    file(TO_CMAKE_PATH "${_boost_root}" _boost_root_norm)
    list(APPEND _BOOST_ROOTS "${_boost_root_norm}")
  endif()
endforeach()
list(REMOVE_DUPLICATES _BOOST_ROOTS)

find_path(Boost_INCLUDE_DIR
  NAMES boost/version.hpp
  PATHS
    ${_BOOST_ROOTS}
  PATH_SUFFIXES
    ""
    include
    include/boost-1_83
    include/boost-1_84
    include/boost-1_85
    include/boost-1_86
  DOC "Path to the Boost include directory containing boost/version.hpp"
)

set(Boost_INCLUDE_DIRS "${Boost_INCLUDE_DIR}")

# Determine a useful root from the include dir if the caller did not provide one.
set(_BOOST_EFFECTIVE_ROOTS ${_BOOST_ROOTS})
if(Boost_INCLUDE_DIR)
  get_filename_component(_boost_include_parent "${Boost_INCLUDE_DIR}" DIRECTORY)
  list(APPEND _BOOST_EFFECTIVE_ROOTS "${Boost_INCLUDE_DIR}" "${_boost_include_parent}")
endif()
list(REMOVE_DUPLICATES _BOOST_EFFECTIVE_ROOTS)

set(_BOOST_LIBRARY_DIRS
  "${Boost_LIBRARYDIR}"
  "${BOOST_LIBRARYDIR}"
  "$ENV{Boost_LIBRARYDIR}"
  "$ENV{BOOST_LIBRARYDIR}"
)
foreach(_boost_root ${_BOOST_EFFECTIVE_ROOTS})
  if(_boost_root)
    list(APPEND _BOOST_LIBRARY_DIRS
      "${_boost_root}/lib64-msvc-14.3"
      "${_boost_root}/lib64-msvc-14.2"
      "${_boost_root}/lib64-msvc-14.1"
      "${_boost_root}/lib64-msvc-14.0"
      "${_boost_root}/lib32-msvc-14.3"
      "${_boost_root}/lib32-msvc-14.2"
      "${_boost_root}/lib"
      "${_boost_root}/stage/lib")
  endif()
endforeach()
list(REMOVE_DUPLICATES _BOOST_LIBRARY_DIRS)

# Parse BOOST_VERSION from boost/version.hpp (e.g. 108300 for 1.83.0).
if(Boost_INCLUDE_DIR AND EXISTS "${Boost_INCLUDE_DIR}/boost/version.hpp")
  file(STRINGS "${Boost_INCLUDE_DIR}/boost/version.hpp" _boost_version_line REGEX "#define[ \t]+BOOST_VERSION[ \t]+[0-9]+")
  string(REGEX REPLACE ".*BOOST_VERSION[ \t]+([0-9]+).*" "\\1" Boost_VERSION "${_boost_version_line}")
  math(EXPR _boost_version_major "${Boost_VERSION} / 100000")
  math(EXPR _boost_version_minor "(${Boost_VERSION} / 100) % 1000")
  math(EXPR _boost_version_patch "${Boost_VERSION} % 100")
  set(Boost_VERSION_STRING "${_boost_version_major}.${_boost_version_minor}.${_boost_version_patch}")
  set(Boost_VERSION_MACRO "${Boost_VERSION}")
endif()

# Respect requested minimum version.
set(_BOOST_VERSION_OK TRUE)
if(Boost_FIND_VERSION AND Boost_VERSION_STRING)
  if(Boost_VERSION_STRING VERSION_LESS Boost_FIND_VERSION)
    set(_BOOST_VERSION_OK FALSE)
  endif()
endif()

function(_psychocore_find_boost_component component)
  string(TOUPPER "${component}" component_upper)
  string(REPLACE "-" "_" component_upper "${component_upper}")

  set(_names
    "libboost_${component}-vc143-mt-x64-1_83"
    "boost_${component}-vc143-mt-x64-1_83"
    "libboost_${component}-vc142-mt-x64-1_83"
    "boost_${component}-vc142-mt-x64-1_83"
    "libboost_${component}-vc141-mt-x64-1_83"
    "boost_${component}-vc141-mt-x64-1_83"
    "libboost_${component}-vc140-mt-x64-1_83"
    "boost_${component}-vc140-mt-x64-1_83"
    "libboost_${component}-vc143-mt-gd-x64-1_83"
    "boost_${component}-vc143-mt-gd-x64-1_83"
    "libboost_${component}-vc142-mt-gd-x64-1_83"
    "boost_${component}-vc142-mt-gd-x64-1_83"
    "libboost_${component}"
    "boost_${component}"
  )

  find_library(Boost_${component_upper}_LIBRARY
    NAMES ${_names}
    PATHS ${_BOOST_LIBRARY_DIRS}
    DOC "Boost ${component} library"
  )

  if(Boost_${component_upper}_LIBRARY)
    set(Boost_${component_upper}_FOUND TRUE PARENT_SCOPE)
    set(Boost_${component_upper}_LIBRARY "${Boost_${component_upper}_LIBRARY}" PARENT_SCOPE)
  else()
    set(Boost_${component_upper}_FOUND FALSE PARENT_SCOPE)
  endif()
endfunction()

set(Boost_LIBRARIES "")
set(_BOOST_MISSING_COMPONENTS "")
foreach(_boost_component ${Boost_FIND_COMPONENTS})
  _psychocore_find_boost_component("${_boost_component}")
  string(TOUPPER "${_boost_component}" _boost_component_upper)
  string(REPLACE "-" "_" _boost_component_upper "${_boost_component_upper}")
  if(Boost_${_boost_component_upper}_LIBRARY)
    list(APPEND Boost_LIBRARIES "${Boost_${_boost_component_upper}_LIBRARY}")
  else()
    list(APPEND _BOOST_MISSING_COMPONENTS "${_boost_component}")
  endif()
endforeach()

if(Boost_INCLUDE_DIR AND _BOOST_VERSION_OK AND NOT _BOOST_MISSING_COMPONENTS)
  set(Boost_FOUND TRUE)
else()
  set(Boost_FOUND FALSE)
endif()

if(Boost_FOUND)
  if(Boost_FIND_COMPONENTS)
    message(STATUS "Found Boost ${Boost_VERSION_STRING}: ${Boost_INCLUDE_DIR} components: ${Boost_FIND_COMPONENTS}")
  else()
    message(STATUS "Found Boost ${Boost_VERSION_STRING}: ${Boost_INCLUDE_DIR}")
  endif()
else()
  if(NOT Boost_INCLUDE_DIR)
    set(_boost_reason "Boost headers not found. Set BOOST_ROOT to your Boost 1.83 folder, e.g. C:/local/boost_1_83_0.")
  elseif(NOT _BOOST_VERSION_OK)
    set(_boost_reason "Boost version ${Boost_VERSION_STRING} is older than requested ${Boost_FIND_VERSION}.")
  elseif(_BOOST_MISSING_COMPONENTS)
    set(_boost_reason "Missing Boost libraries/components: ${_BOOST_MISSING_COMPONENTS}. Check BOOST_LIBRARYDIR or build Boost libraries with b2.")
  else()
    set(_boost_reason "Unknown Boost lookup failure.")
  endif()

  if(Boost_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find Boost. ${_boost_reason}")
  else()
    message(STATUS "Could not find Boost. ${_boost_reason}")
  endif()
endif()

mark_as_advanced(Boost_INCLUDE_DIR Boost_INCLUDE_DIRS Boost_LIBRARIES)
