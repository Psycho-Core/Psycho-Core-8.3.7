#
# Find the MySQL client includes and library
#

# This module defines
# MYSQL_INCLUDE_DIR, where to find mysql.h
# MYSQL_LIBRARIES, the libraries to link against to connect to MySQL
# MYSQL_EXECUTABLE, the MySQL executable.
# MYSQL_FOUND, if false, you cannot build anything that requires MySQL.

# also defined, but not for general use are
# MYSQL_LIBRARY, where to find the MySQL library.

set( MYSQL_FOUND 0 )

# Psycho_Core bundled MariaDB support
# -----------------------------------
# Windows builds for this project keep the UNZIPPED/EXTRACTED MariaDB client
# package under dep/mysql:
#   <Psycho_Core-8.3.7>/dep/mysql
# Use CMAKE_SOURCE_DIR instead of a hardcoded drive/user path so the same layout
# works for every user, even when the repo is cloned somewhere else.
#
# Supported layouts:
#   dep/mysql/include, dep/mysql/lib, dep/mysql/bin
#   dep/mysql/mariadb-11.8.6-winx64/include, lib, bin  (if the extracted folder
#                                                       is placed under dep/mysql)
set(PSYCHOCORE_BUNDLED_MARIADB_ROOT
  "${CMAKE_SOURCE_DIR}/dep/mysql"
)
set(PSYCHOCORE_BUNDLED_MARIADB_INCLUDE_PATHS
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/include"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/include/mysql"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/mariadb-11.8.6-winx64/include"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/mariadb-11.8.6-winx64/include/mysql"
)
set(PSYCHOCORE_BUNDLED_MARIADB_LIBRARY_PATHS
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/lib"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/lib/opt"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/lib/mariadb"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/mariadb-11.8.6-winx64/lib"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/mariadb-11.8.6-winx64/lib/opt"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/mariadb-11.8.6-winx64/lib/mariadb"
)
set(PSYCHOCORE_BUNDLED_MARIADB_BINARY_PATHS
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/bin"
  "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/mariadb-11.8.6-winx64/bin"
)

if(WIN32 AND EXISTS "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}")
  message(STATUS "Using bundled MariaDB search root: ${PSYCHOCORE_BUNDLED_MARIADB_ROOT}")

  # UNCONDITIONAL auto-detect of the in-tree MariaDB client (portable build).
  # If mysql.h and the import lib exist ANYWHERE under dep/mysql, FORCE-set the cache
  # variables so they override stale cache / system MySQL Server installs and the user
  # never has to set MYSQL_INCLUDE_DIR / MYSQL_LIBRARY manually.
  # Searches both dep/mysql and dep/mysql/mariadb-11.8.6-winx64 layouts.
  foreach(_root "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}" "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/mariadb-11.8.6-winx64")
    # Headers
    if(EXISTS "${_root}/include/mysql.h" AND NOT MYSQL_INCLUDE_DIR)
      set(MYSQL_INCLUDE_DIR "${_root}/include" CACHE PATH "MySQL include dir" FORCE)
      message(STATUS "Auto-detected MySQL headers: ${MYSQL_INCLUDE_DIR}")
    endif()
    # Library — searched independently of headers, at common lib sub-paths
    foreach(_libsubdir lib lib/opt lib/mariadb)
      foreach(_libname libmariadb libmysql mariadbclient)
        if(EXISTS "${_root}/${_libsubdir}/${_libname}.lib")
          set(MYSQL_LIBRARY "${_root}/${_libsubdir}/${_libname}.lib" CACHE FILEPATH "MySQL library" FORCE)
          message(STATUS "Auto-detected MySQL library: ${MYSQL_LIBRARY}")
          break()
        endif()
      endforeach()
      if(MYSQL_LIBRARY)
        break()
      endif()
    endforeach()
    if(MYSQL_LIBRARY)
      break()
    endif()
  endforeach()
endif()

if( UNIX )
  set(MYSQL_CONFIG_PREFER_PATH "$ENV{MYSQL_HOME}/bin" CACHE FILEPATH
    "preferred path to MySQL (mysql_config)"
  )

  find_program(MYSQL_CONFIG mysql_config
    ${MYSQL_CONFIG_PREFER_PATH}
    /usr/local/mysql/bin/
    /usr/local/bin/
    /usr/bin/
  )

  if( MYSQL_CONFIG )
    message(STATUS "Using mysql-config: ${MYSQL_CONFIG}")
    # set INCLUDE_DIR
    execute_process(COMMAND ${MYSQL_CONFIG} --include
      OUTPUT_VARIABLE MY_TMP
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(REGEX REPLACE "-I([^ ]*)( .*)?" "\\1" MY_TMP "${MY_TMP}")
    set(MYSQL_ADD_INCLUDE_PATH ${MY_TMP} CACHE FILEPATH INTERNAL)
    #message("[DEBUG] MYSQL ADD_INCLUDE_PATH : ${MYSQL_ADD_INCLUDE_PATH}")
    # set LIBRARY_DIR
    execute_process(COMMAND ${MYSQL_CONFIG} --libs_r
      OUTPUT_VARIABLE MY_TMP
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(MYSQL_ADD_LIBRARIES "")
    string(REGEX MATCHALL "-l[^ ]*" MYSQL_LIB_LIST "${MY_TMP}")
    foreach(LIB ${MYSQL_LIB_LIST})
      string(REGEX REPLACE "[ ]*-l([^ ]*)" "\\1" LIB "${LIB}")
      list(APPEND MYSQL_ADD_LIBRARIES "${LIB}")
      #message("[DEBUG] MYSQL ADD_LIBRARIES : ${MYSQL_ADD_LIBRARIES}")
    endforeach(LIB ${MYSQL_LIB_LIST})

    set(MYSQL_ADD_LIBRARIES_PATH "")
    string(REGEX MATCHALL "-L[^ ]*" MYSQL_LIBDIR_LIST "${MY_TMP}")
    foreach(LIB ${MYSQL_LIBDIR_LIST})
      string(REGEX REPLACE "[ ]*-L([^ ]*)" "\\1" LIB "${LIB}")
      list(APPEND MYSQL_ADD_LIBRARIES_PATH "${LIB}")
      #message("[DEBUG] MYSQL ADD_LIBRARIES_PATH : ${MYSQL_ADD_LIBRARIES_PATH}")
    endforeach(LIB ${MYSQL_LIBS})

  else( MYSQL_CONFIG )
    set(MYSQL_ADD_LIBRARIES "")
    list(APPEND MYSQL_ADD_LIBRARIES "mysqlclient_r")
  endif( MYSQL_CONFIG )
endif( UNIX )

if( WIN32 )
  # read environment variables and change \ to /
  SET(PROGRAM_FILES_32 $ENV{ProgramFiles})
  if (${PROGRAM_FILES_32})
    STRING(REPLACE "\\\\" "/" PROGRAM_FILES_32 ${PROGRAM_FILES_32})
  endif(${PROGRAM_FILES_32})

  SET(PROGRAM_FILES_64 $ENV{ProgramW6432})
  if (${PROGRAM_FILES_64})
     STRING(REPLACE "\\\\" "/" PROGRAM_FILES_64 ${PROGRAM_FILES_64})
  endif(${PROGRAM_FILES_64})
endif ( WIN32 )

find_path(MYSQL_INCLUDE_DIR
  NAMES
    mysql.h
  PATHS
    ${MYSQL_INCLUDE_DIR}
    $ENV{MYSQL_INCLUDE_DIR}
    ${PSYCHOCORE_BUNDLED_MARIADB_INCLUDE_PATHS}
    ${MYSQL_ADD_INCLUDE_PATH}
    # MariaDB Connector C (common Windows install locations)
    "${PROGRAM_FILES_64}/MariaDB/MariaDB Connector C 3.3/include"
    "${PROGRAM_FILES_64}/MariaDB/MariaDB Connector C 3.2/include"
    "${PROGRAM_FILES_64}/MariaDB/MariaDB Connector C 3.1/include"
    "${PROGRAM_FILES_64}/MariaDB Connector C/include"
    "${PROGRAM_FILES_64}/MariaDB Connector C 64-bit/include"
    "${PROGRAM_FILES_32}/MariaDB/MariaDB Connector C 3.3/include"
    "${PROGRAM_FILES_32}/MariaDB/MariaDB Connector C 3.2/include"
    "${PROGRAM_FILES_32}/MariaDB Connector C/include"
    "${PROGRAM_FILES_32}/MariaDB Connector C 32-bit/include"
    /usr/include
    /usr/include/mysql
    /usr/local/include
    /usr/local/include/mysql
    /usr/local/mysql/include
    "${PROGRAM_FILES_64}/MySQL/MySQL Server 8.0/include"
    "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.7/include"
    "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.6/include"
    "${PROGRAM_FILES_64}/MySQL/include"
    "${PROGRAM_FILES_32}/MySQL/MySQL Server 8.0/include"
    "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.7/include"
    "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.6/include"
    "${PROGRAM_FILES_32}/MySQL/include"
    "C:/MySQL/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 8.0;Location]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.7;Location]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.6;Location]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 8.0;Location]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.7;Location]/include"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.6;Location]/include"
    "$ENV{ProgramFiles}/MySQL/MySQL Server 8.0/include"
    "$ENV{ProgramFiles}/MySQL/MySQL Server 5.7/include"
    "$ENV{ProgramFiles}/MySQL/MySQL Server 5.6/include"
    "$ENV{SystemDrive}/MySQL/MySQL Server 8.0/include"
    "$ENV{SystemDrive}/MySQL/MySQL Server 5.7/include"
    "$ENV{SystemDrive}/MySQL/MySQL Server 5.6/include"
    "c:/msys/local/include"
    "$ENV{MYSQL_ROOT}/include"
  DOC
    "Specify the directory containing mysql.h."
)

if( UNIX )
  foreach(LIB ${MYSQL_ADD_LIBRARIES})
    find_library( MYSQL_LIBRARY
      NAMES
        mysql libmysql libmariadb mariadbclient ${LIB}
      PATHS
        ${PSYCHOCORE_BUNDLED_MARIADB_LIBRARY_PATHS}
        ${MYSQL_ADD_LIBRARIES_PATH}
        /usr/lib
        /usr/lib/mysql
        /usr/local/lib
        /usr/local/lib/mysql
        /usr/local/mysql/lib
      DOC "Specify the location of the mysql library here."
    )
  endforeach(LIB ${MYSQL_ADD_LIBRARY})
endif( UNIX )

if( WIN32 )
  find_library( MYSQL_LIBRARY
    NAMES
      libmysql
      libmariadb
      mariadbclient
      mysqlclient
    PATHS
      ${PSYCHOCORE_BUNDLED_MARIADB_LIBRARY_PATHS}
      ${MYSQL_ADD_LIBRARIES_PATH}
      "${PROGRAM_FILES_64}/MySQL/MySQL Server 8.0/lib"
      "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.7/lib"
      "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.6/lib"
      "${PROGRAM_FILES_64}/MySQL/MySQL Server 8.0/lib/opt"
      "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.7/lib/opt"
      "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.6/lib/opt"
      "${PROGRAM_FILES_64}/MySQL/lib"
      "${PROGRAM_FILES_32}/MySQL/MySQL Server 8.0/lib"
      "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.7/lib"
      "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.6/lib"
      "${PROGRAM_FILES_32}/MySQL/MySQL Server 8.0/lib/opt"
      "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.7/lib/opt"
      "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.6/lib/opt"
      "${PROGRAM_FILES_32}/MySQL/lib"
      "C:/MySQL/lib/debug"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 8.0;Location]/lib"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.7;Location]/lib"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.6;Location]/lib"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 8.0;Location]/lib/opt"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.7;Location]/lib/opt"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.6;Location]/lib/opt"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 8.0;Location]/lib"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.7;Location]/lib"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.6;Location]/lib"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 8.0;Location]/lib/opt"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.7;Location]/lib/opt"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.6;Location]/lib/opt"
      "$ENV{ProgramFiles}/MySQL/MySQL Server 8.0/lib/opt"
      "$ENV{ProgramFiles}/MySQL/MySQL Server 5.7/lib/opt"
      "$ENV{ProgramFiles}/MySQL/MySQL Server 5.6/lib/opt"
      "$ENV{SystemDrive}/MySQL/MySQL Server 8.0/lib/opt"
      "$ENV{SystemDrive}/MySQL/MySQL Server 5.7/lib/opt"
      "$ENV{SystemDrive}/MySQL/MySQL Server 5.6/lib/opt"
      "c:/msys/local/include"
      "$ENV{MYSQL_ROOT}/lib"
    DOC "Specify the location of the mysql library here."
  )
endif( WIN32 )

# On Windows you typically don't need to include any extra libraries
# to build MYSQL stuff.

if( NOT WIN32 )
  find_library( MYSQL_EXTRA_LIBRARIES
    NAMES
      z zlib
    PATHS
      /usr/lib
      /usr/local/lib
    DOC
      "if more libraries are necessary to link in a MySQL client (typically zlib), specify them here."
  )
else( NOT WIN32 )
  set( MYSQL_EXTRA_LIBRARIES "" )
endif( NOT WIN32 )

if( UNIX )
    find_program(MYSQL_EXECUTABLE mysql
    PATHS
        ${MYSQL_CONFIG_PREFER_PATH}
        /usr/local/mysql/bin/
        /usr/local/bin/
        /usr/bin/
    DOC
        "path to your mysql binary."
    )
endif( UNIX )

if( WIN32 )
    find_program(MYSQL_EXECUTABLE mysql
      PATHS
        ${PSYCHOCORE_BUNDLED_MARIADB_BINARY_PATHS}
        "${PROGRAM_FILES_64}/MySQL/MySQL Server 8.0/bin"
        "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.7/bin"
        "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.6/bin"
        "${PROGRAM_FILES_64}/MySQL/MySQL Server 8.0/bin/opt"
        "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.7/bin/opt"
        "${PROGRAM_FILES_64}/MySQL/MySQL Server 5.6/bin/opt"
        "${PROGRAM_FILES_64}/MySQL/bin"
        "${PROGRAM_FILES_32}/MySQL/MySQL Server 8.0/bin"
        "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.7/bin"
        "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.6/bin"
        "${PROGRAM_FILES_32}/MySQL/MySQL Server 8.0/bin/opt"
        "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.7/bin/opt"
        "${PROGRAM_FILES_32}/MySQL/MySQL Server 5.6/bin/opt"
        "${PROGRAM_FILES_32}/MySQL/bin"
        "C:/MySQL/bin/debug"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 8.0;Location]/bin"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.7;Location]/bin"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.6;Location]/bin"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 8.0;Location]/bin/opt"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.7;Location]/bin/opt"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.6;Location]/bin/opt"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 8.0;Location]/bin"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.7;Location]/bin"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.6;Location]/bin"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 8.0;Location]/bin/opt"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.7;Location]/bin/opt"
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.6;Location]/bin/opt"
        "$ENV{ProgramFiles}/MySQL/MySQL Server 8.0/bin/opt"
        "$ENV{ProgramFiles}/MySQL/MySQL Server 5.7/bin/opt"
        "$ENV{ProgramFiles}/MySQL/MySQL Server 5.6/bin/opt"
        "$ENV{SystemDrive}/MySQL/MySQL Server 8.0/bin/opt"
        "$ENV{SystemDrive}/MySQL/MySQL Server 5.7/bin/opt"
        "$ENV{SystemDrive}/MySQL/MySQL Server 5.6/bin/opt"
        "c:/msys/local/include"
        "$ENV{MYSQL_ROOT}/bin"
     DOC
        "path to your mysql binary."
    )
endif( WIN32 )

if( MYSQL_LIBRARY )
  if( MYSQL_INCLUDE_DIR )
    set( MYSQL_FOUND 1 )

    # Final override: if the bundled MariaDB client lib exists under dep/mysql, it
    # ALWAYS wins over any system MySQL Server install that find_library may have
    # picked up. Re-verify and FORCE-set so the user never gets the wrong library.
    foreach(_root "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}" "${PSYCHOCORE_BUNDLED_MARIADB_ROOT}/mariadb-11.8.6-winx64")
      foreach(_libsubdir lib lib/opt lib/mariadb)
        foreach(_libname libmariadb libmysql mariadbclient)
          if(EXISTS "${_root}/${_libsubdir}/${_libname}.lib")
            get_filename_component(_current_lib "${MYSQL_LIBRARY}" NAME)
            if(NOT "${_current_lib}" STREQUAL "${_libname}.lib" OR NOT "${MYSQL_LIBRARY}" STREQUAL "${_root}/${_libsubdir}/${_libname}.lib")
              set(MYSQL_LIBRARY "${_root}/${_libsubdir}/${_libname}.lib" CACHE FILEPATH "MySQL library" FORCE)
              message(STATUS "Overriding MySQL library with bundled: ${MYSQL_LIBRARY}")
            endif()
            break()
          endif()
        endforeach()
        if(EXISTS "${_root}/${_libsubdir}/libmariadb.lib" OR EXISTS "${_root}/${_libsubdir}/libmysql.lib")
          break()
        endif()
      endforeach()
      if(EXISTS "${_root}/lib/libmariadb.lib" OR EXISTS "${_root}/lib/libmysql.lib")
        break()
      endif()
    endforeach()

    message(STATUS "Found MySQL library: ${MYSQL_LIBRARY}")
    message(STATUS "Found MySQL headers: ${MYSQL_INCLUDE_DIR}")
  else( MYSQL_INCLUDE_DIR )
    message(FATAL_ERROR
      "Could not find MySQL headers (mysql.h)!\n"
      "The 'database' project needs the MariaDB/MySQL client development headers.\n"
      "Fix ONE of these:\n"
      "  1) Unzip dep/mysql/mariadb-11.8.6-winx64.zip so that\n"
      "     dep/mysql/mariadb-11.8.6-winx64/include/mysql.h exists, OR\n"
      "  2) Install 'MariaDB Connector C' and let FindMySQL auto-detect it, OR\n"
      "  3) Set MYSQL_INCLUDE_DIR to the folder that contains mysql.h, e.g.\n"
      "     cmake -DMYSQL_INCLUDE_DIR=\"C:/Program Files/MariaDB/MariaDB Connector C 3.3/include\" ...\n"
      "Also set MYSQL_LIBRARY (or MYSQL_ADD_LIBRARIES_PATH) to the matching lib folder.")
  endif( MYSQL_INCLUDE_DIR )
  if( MYSQL_EXECUTABLE )
    message(STATUS "Found MySQL executable: ${MYSQL_EXECUTABLE}")
  endif( MYSQL_EXECUTABLE )
  mark_as_advanced( MYSQL_FOUND MYSQL_LIBRARY MYSQL_EXTRA_LIBRARIES MYSQL_INCLUDE_DIR MYSQL_EXECUTABLE)
else( MYSQL_LIBRARY )
  message(FATAL_ERROR "Could not find the MySQL libraries! Please install the development libraries and headers")
endif( MYSQL_LIBRARY )
