===========================================================================
 Psycho_Core - bundled MariaDB client location for Windows builds
===========================================================================

This folder is where Windows users put the MariaDB client package that CMake
uses to find mysql.h and libmariadb/libmysql.

IMPORTANT: CMake cannot use a .zip file directly. You must UNZIP/EXTRACT the
MariaDB Windows x64 package first.

Preferred CMake/build layout:

  dep\mysql\include\mysql.h
  dep\mysql\lib\libmariadb.lib      (or libmysql.lib, depending on package)
  dep\mysql\bin\libmariadb.dll      (or libmysql.dll, depending on package)

Also supported if your archive extracts with its own top folder:

  dep\mysql\mariadb-11.8.6-winx64\include\mysql.h
  dep\mysql\mariadb-11.8.6-winx64\lib\libmariadb.lib
  dep\mysql\mariadb-11.8.6-winx64\bin\libmariadb.dll

For a standalone portable runtime server, copy the whole mysql folder to your
server root, for example:

  .\mysql

Then follow the full portable setup guide in this folder:

  PORTABLE_MYSQL_SERVER_SETUP.txt

This folder also includes portable helper files:

  my.ini
  init_mysql.bat
  start_mysql.bat
  stop_mysql.bat
  mysql_console.bat
  setup_psycho_databases.bat
  apply_portable_myini.bat
  create_psycho_databases.sql

Use the .bat files first. From your server root, use:

  mysql\init_mysql.bat
  mysql\start_mysql.bat
  mysql\setup_psycho_databases.bat
  mysql\mysql_console.bat

The helper scripts go into mysql\bin and use the proven Windows/MariaDB commands:

  mariadb-install-db.exe --datadir=../data --password=...
  copy /Y ..\my.ini ..\data\my.ini
  mysqld.exe --defaults-file=../data/my.ini --console

MariaDB may generate mysql\data\my.ini during initialization. That is normal;
our scripts replace it with the Psycho_Core portable config.

Manual commands are documented in PORTABLE_MYSQL_SERVER_SETUP.txt only as fallback.

After changing this folder, delete your CMake cache/build folder before
configuring again:

  Psycho_Core-8.3.7\bld\CMakeCache.txt

or delete the whole:

  Psycho_Core-8.3.7\bld

CMake search root used by cmake/macros/FindMySQL.cmake:

  ${CMAKE_SOURCE_DIR}/dep/mysql
===========================================================================
