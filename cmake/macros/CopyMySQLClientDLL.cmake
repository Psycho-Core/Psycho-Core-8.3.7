# ===========================================================================
# CopyMySQLClientDLL.cmake
# -------------------------------------------------------------------------
# Copies the MariaDB/MySQL client runtime DLL (libmariadb.dll / libmysql.dll)
# next to a built executable, so the server can load it at runtime without the
# user having to copy it manually.
#
# It searches the bundled dep/mysql locations first (portable build), then the
# directory of MYSQL_LIBRARY. No-op on non-Windows or when no DLL is found.
# ===========================================================================

function(CopyMySQLClientDLL target)
  if(NOT WIN32)
    return()
  endif()

  set(_MYSQL_DLL_SEARCH_PATHS
    "${CMAKE_SOURCE_DIR}/dep/mysql/bin"
    "${CMAKE_SOURCE_DIR}/dep/mysql/mariadb-11.8.6-winx64/bin"
    "${CMAKE_SOURCE_DIR}/dep/mysql/lib"
    "${CMAKE_SOURCE_DIR}/dep/mysql/mariadb-11.8.6-winx64/lib"
    "${CMAKE_SOURCE_DIR}/dep/mysql/lib/mariadb"
    "${CMAKE_SOURCE_DIR}/dep/mysql/mariadb-11.8.6-winx64/lib/mariadb")

  # Also look relative to the import library if it was found.
  if(MYSQL_LIBRARY)
    get_filename_component(_MYSQL_LIB_DIR "${MYSQL_LIBRARY}" DIRECTORY)
    list(APPEND _MYSQL_DLL_SEARCH_PATHS "${_MYSQL_LIB_DIR}")
    # ..\bin next to ..\lib
    list(APPEND _MYSQL_DLL_SEARCH_PATHS "${_MYSQL_LIB_DIR}/../bin")
  endif()

  foreach(_dllname libmariadb libmysql mysqlclient)
    find_file(_MYSQL_CLIENT_DLL
      NAMES "${_dllname}.dll"
      PATHS ${_MYSQL_DLL_SEARCH_PATHS}
      NO_DEFAULT_PATH
      DOC "MariaDB/MySQL client runtime DLL"
    )
    if(_MYSQL_CLIENT_DLL)
      break()
    endif()
  endforeach()

  if(_MYSQL_CLIENT_DLL)
    if("${CMAKE_MAKE_PROGRAM}" MATCHES "MSBuild")
      add_custom_command(TARGET ${target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_MYSQL_CLIENT_DLL}" "${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/"
        COMMENT "Copying ${_dllname}.dll next to ${target}"
      )
    else()
      add_custom_command(TARGET ${target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_MYSQL_CLIENT_DLL}" "${CMAKE_BINARY_DIR}/bin/"
        COMMENT "Copying ${_dllname}.dll next to ${target}"
      )
    endif()
  endif()

  set(_MYSQL_CLIENT_DLL "" CACHE INTERNAL "")
endfunction()
