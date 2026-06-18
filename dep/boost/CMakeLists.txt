# This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if(WIN32)
  set(BOOST_DEBUG ON)

  # UNCONDITIONAL, FIRST: auto-detect the portable in-tree Boost under dep/boost.
  # If boost/version.hpp exists there, FORCE-set BOOST_ROOT + Boost_INCLUDE_DIR so it
  # overrides any stale env var (e.g. an old 1.70.0 system Boost) and the user never
  # has to set anything manually. Only falls through to env-var logic if dep/boost is empty.
  if(EXISTS "${CMAKE_SOURCE_DIR}/dep/boost/boost/version.hpp")
    set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/dep/boost" CACHE PATH "Boost root" FORCE)
    set(Boost_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/dep/boost" CACHE PATH "Boost include dir" FORCE)
    message(STATUS "Auto-detected Boost in dep/boost (portable in-tree install).")
  elseif(DEFINED ENV{BOOST_ROOT})
    set(BOOST_ROOT $ENV{BOOST_ROOT})
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0)
      set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib${PLATFORM}-msvc-12.0)
    elseif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.10)
      set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib${PLATFORM}-msvc-14.0)
    else()
      list(APPEND BOOST_LIBRARYDIR
        ${BOOST_ROOT}/lib${PLATFORM}-msvc-14.3
        ${BOOST_ROOT}/lib${PLATFORM}-msvc-14.2
        ${BOOST_ROOT}/lib${PLATFORM}-msvc-14.1
        ${BOOST_ROOT}/lib${PLATFORM}-msvc-14.0
        ${BOOST_ROOT}/stage/lib
        ${BOOST_ROOT}/lib)
    endif()
  elseif(EXISTS "C:/local/boost_1_83_0/boost/version.hpp")
    set(BOOST_ROOT "C:/local/boost_1_83_0" CACHE PATH "Boost root" FORCE)
  elseif(EXISTS "C:/boost_1_83_0/boost/version.hpp")
    set(BOOST_ROOT "C:/boost_1_83_0" CACHE PATH "Boost root" FORCE)
  else()
    message(STATUS "BOOST_ROOT was not set; FindBoost will search common paths. If Boost is not found, set BOOST_ROOT to your Boost 1.83 folder.")
  endif()

  set(Boost_USE_STATIC_LIBS        ON)
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME     OFF)
  # Resolve Release (mt) Boost libs only. Mixing a Debug Boost lib into a
  # Release build triggers LNK2038 RuntimeLibrary/IDL mismatches.
  set(Boost_USE_RELEASE_LIBS       ON)
  set(Boost_USE_DEBUG_LIBS         OFF)
endif()

if (CMAKE_VERSION VERSION_LESS "3.16.0")
  # boost's own cmake file adds extra compile definitions that cotire fails to pick up
  set(Boost_NO_BOOST_CMAKE ON)
endif()

include (CheckCXXSourceCompiles)

# Psycho_Core targets Boost 1.83 on all platforms.
# CMake 4.x no longer ships the old FindBoost module, so the project provides
# cmake/macros/FindBoost.cmake for classic Boost variables.
set(BOOST_REQUIRED_VERSION 1.83)

find_package(Boost ${BOOST_REQUIRED_VERSION} REQUIRED system filesystem thread program_options iostreams regex)

# Find if Boost was compiled in C++03 mode because it requires -DBOOST_NO_CXX11_SCOPED_ENUMS

set(CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})
set(CMAKE_REQUIRED_FLAGS "-std=c++11")
check_cxx_source_compiles("
  #include <boost/filesystem/path.hpp>
  #include <boost/filesystem/operations.hpp>
  int main() { boost::filesystem::copy_file(boost::filesystem::path(), boost::filesystem::path()); }"
boost_filesystem_copy_links_without_NO_SCOPED_ENUM)
unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)
unset(CMAKE_REQUIRED_FLAGS)

add_library(boost INTERFACE)

target_link_libraries(boost
  INTERFACE
    ${Boost_LIBRARIES})

target_include_directories(boost
  INTERFACE
    ${Boost_INCLUDE_DIRS})

target_compile_definitions(boost
  INTERFACE
    -DBOOST_DATE_TIME_NO_LIB
    -DBOOST_REGEX_NO_LIB
    -DBOOST_CHRONO_NO_LIB
	# Due to MSVC linking error boost::none" already defined in scripts_...
    # May be removed when the requirement is raised to boost 1.61 on windows.
    -DBOOST_OPTIONAL_USE_OLD_DEFINITION_OF_NONE
    -DBOOST_SERIALIZATION_NO_LIB
    -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
    -DBOOST_ASIO_NO_DEPRECATED
    -DBOOST_BIND_NO_PLACEHOLDERS)

if (NOT boost_filesystem_copy_links_without_NO_SCOPED_ENUM)
  target_compile_definitions(boost
    INTERFACE
      -DBOOST_NO_CXX11_SCOPED_ENUMS)
endif()

target_compile_definitions(boost
  INTERFACE
    -DTC_HAS_BROKEN_WSTRING_REGEX)
