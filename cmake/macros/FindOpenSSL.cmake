# - Try to find the OpenSSL encryption library

set(OPENSSL_EXPECTED_VERSION "1.1.1")

SET(_OPENSSL_ROOT_HINTS
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL (32-bit)_is1;Inno Setup: App Path]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OpenSSL (64-bit)_is1;Inno Setup: App Path]"
)

# Psycho_Core portable OpenSSL search.
# Order: (1) BUNDLED dep/openssl (portable, relative to repo root via CMAKE_SOURCE_DIR
# so it works from any drive/path with no drive label), then (2) common Windows
# install locations. Drop/extract an OpenSSL 3.5.x build into dep/openssl to make
# the whole source tree portable.
IF(PLATFORM EQUAL 64)
  SET(_OPENSSL_ROOT_PATHS
    "${CMAKE_SOURCE_DIR}/dep/openssl"
    "${CMAKE_SOURCE_DIR}/dep/openssl/OpenSSL-Win64"
    "C:/Program Files/OpenSSL-3_5_6-Win64"
    "C:/OpenSSL-Win64/"
    "C:/OpenSSL/"
  )
ELSE()
  SET(_OPENSSL_ROOT_PATHS
    "${CMAKE_SOURCE_DIR}/dep/openssl"
    "${CMAKE_SOURCE_DIR}/dep/openssl/OpenSSL-Win32"
    "C:/OpenSSL/"
  )
ENDIF()

FIND_PATH(OPENSSL_ROOT_DIR
  NAMES include/openssl/ssl.h
  HINTS ${_OPENSSL_ROOT_HINTS}
  PATHS ${_OPENSSL_ROOT_PATHS}
)
MARK_AS_ADVANCED(OPENSSL_ROOT_DIR)

FIND_PATH(OPENSSL_INCLUDE_DIR
  NAMES openssl/ssl.h
  PATHS ${OPENSSL_ROOT_DIR}/include
  NO_DEFAULT_PATH
)
if(NOT OPENSSL_INCLUDE_DIR)
  FIND_PATH(OPENSSL_INCLUDE_DIR openssl/ssl.h)
endif()

IF(WIN32 AND NOT CYGWIN)
  IF(MSVC)

    # --- OpenSSL 3.x library search paths ---
    set(_OPENSSL_LIB_PATHS
      ${OPENSSL_ROOT_DIR}/lib
      ${OPENSSL_ROOT_DIR}/lib/VC
      ${OPENSSL_ROOT_DIR}/lib/VC/x64
      ${OPENSSL_ROOT_DIR}/lib/VC/x64/MD
      ${OPENSSL_ROOT_DIR}/lib/VC/x64/MT
    )

    # SSL library
    FIND_LIBRARY(OPENSSL_SSL_LIBRARY
      NAMES libssl ssl libssl_static
      PATHS ${_OPENSSL_LIB_PATHS}
    )

    # Crypto library
    FIND_LIBRARY(OPENSSL_CRYPTO_LIBRARY
      NAMES libcrypto crypto libcrypto_static
      PATHS ${_OPENSSL_LIB_PATHS}
    )

    if(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      set(OPENSSL_LIBRARIES
        optimized ${OPENSSL_SSL_LIBRARY} optimized ${OPENSSL_CRYPTO_LIBRARY}
        debug ${OPENSSL_SSL_LIBRARY} debug ${OPENSSL_CRYPTO_LIBRARY}
      )
    else()
      set(OPENSSL_LIBRARIES
        ${OPENSSL_SSL_LIBRARY}
        ${OPENSSL_CRYPTO_LIBRARY}
      )
    endif()

    MARK_AS_ADVANCED(OPENSSL_SSL_LIBRARY OPENSSL_CRYPTO_LIBRARY)

  ENDIF()
ELSE()
  # BfaCore adaptation: Psycho_Core's finder accepted OpenSSL 3.x but on Unix
  # still allowed the system search path to win even when OPENSSL_ROOT_DIR was
  # explicitly provided. Prefer OPENSSL_ROOT_DIR first so configure can target
  # a local OpenSSL 3.5.x extraction without touching system packages.
  set(_OPENSSL_UNIX_ROOT_PATHS "")
  if(OPENSSL_ROOT_DIR)
    list(APPEND _OPENSSL_UNIX_ROOT_PATHS
      "${OPENSSL_ROOT_DIR}/lib"
      "${OPENSSL_ROOT_DIR}/lib64"
      "${OPENSSL_ROOT_DIR}/lib/x86_64-linux-gnu"
      "${OPENSSL_ROOT_DIR}/lib/aarch64-linux-gnu")
  endif()

  FIND_LIBRARY(OPENSSL_SSL_LIBRARIES
    NAMES ssl
    PATHS ${_OPENSSL_UNIX_ROOT_PATHS}
    NO_DEFAULT_PATH)
  FIND_LIBRARY(OPENSSL_CRYPTO_LIBRARIES
    NAMES crypto
    PATHS ${_OPENSSL_UNIX_ROOT_PATHS}
    NO_DEFAULT_PATH)

  if(NOT OPENSSL_SSL_LIBRARIES)
    FIND_LIBRARY(OPENSSL_SSL_LIBRARIES NAMES ssl)
  endif()
  if(NOT OPENSSL_CRYPTO_LIBRARIES)
    FIND_LIBRARY(OPENSSL_CRYPTO_LIBRARIES NAMES crypto)
  endif()

  SET(OPENSSL_LIBRARIES ${OPENSSL_SSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES})
ENDIF()

if(NOT OPENSSL_INCLUDE_DIR)
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(OpenSSL DEFAULT_MSG
    OPENSSL_LIBRARIES
    OPENSSL_INCLUDE_DIR
  )
endif()

if(OPENSSL_INCLUDE_DIR)
  message(STATUS "Found OpenSSL library: ${OPENSSL_LIBRARIES}")
  message(STATUS "Found OpenSSL headers: ${OPENSSL_INCLUDE_DIR}")

  # -------------------------------
  # Extract OpenSSL version (3.x + 1.1.1 compatible)
  # -------------------------------

  # Try OpenSSL 3.x macro
  file(STRINGS "${OPENSSL_INCLUDE_DIR}/openssl/opensslv.h" OPENSSL_VERSION_LINE
       REGEX "^# *define OPENSSL_VERSION_STR.*")

  if(OPENSSL_VERSION_LINE)
      string(REGEX REPLACE "^# *define OPENSSL_VERSION_STR *\"([^\"]+)\".*"
             "\\1" OPENSSL_VERSION "${OPENSSL_VERSION_LINE}")
  else()
      # Fallback for OpenSSL 1.1.1
      file(STRINGS "${OPENSSL_INCLUDE_DIR}/openssl/opensslv.h" openssl_version_str
           REGEX "^# *define[\t ]+OPENSSL_VERSION_NUMBER[\t ]+0x[0-9A-Fa-f]+")

      string(REGEX REPLACE "^.*0x([0-9A-Fa-f]).*$" "\\1" OPENSSL_VERSION_MAJOR "${openssl_version_str}")
      string(REGEX REPLACE "^.*0x[0-9A-Fa-f]([0-9A-Fa-f][0-9A-Fa-f]).*$" "\\1" OPENSSL_VERSION_MINOR "${openssl_version_str}")
      string(REGEX REPLACE "^.*0x[0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]([0-9A-Fa-f][0-9A-Fa-f]).*$" "\\1" OPENSSL_VERSION_FIX "${openssl_version_str}")

      string(REGEX REPLACE "^0" "" OPENSSL_VERSION_MINOR "${OPENSSL_VERSION_MINOR}")
      string(REGEX REPLACE "^0" "" OPENSSL_VERSION_FIX "${OPENSSL_VERSION_FIX}")

      set(OPENSSL_VERSION "${OPENSSL_VERSION_MAJOR}.${OPENSSL_VERSION_MINOR}.${OPENSSL_VERSION_FIX}")
  endif()

  message(STATUS "Detected OpenSSL version: ${OPENSSL_VERSION}")

  # -------------------------------
  # Numeric version check (OpenSSL 1.1.1 → 3.x)
  # -------------------------------

  string(REGEX REPLACE "\\." ";" OPENSSL_VERSION_LIST "${OPENSSL_VERSION}")
  list(GET OPENSSL_VERSION_LIST 0 OPENSSL_VER_MAJOR)
  list(GET OPENSSL_VERSION_LIST 1 OPENSSL_VER_MINOR)
  list(GET OPENSSL_VERSION_LIST 2 OPENSSL_VER_PATCH)

  math(EXPR OPENSSL_VERSION_NUM "${OPENSSL_VER_MAJOR} * 1000000 + ${OPENSSL_VER_MINOR} * 1000 + ${OPENSSL_VER_PATCH}")

  # Minimum required: 1.1.1 → 1001001
  set(OPENSSL_MIN_VERSION_NUM 1001001)

  if(OPENSSL_VERSION_NUM LESS OPENSSL_MIN_VERSION_NUM)
      message(FATAL_ERROR "TrinityCore needs OpenSSL >= 1.1.1 but found ${OPENSSL_VERSION}")
  endif()

  message(STATUS "OpenSSL version accepted: ${OPENSSL_VERSION}")
endif()

MARK_AS_ADVANCED(OPENSSL_INCLUDE_DIR OPENSSL_LIBRARIES)
