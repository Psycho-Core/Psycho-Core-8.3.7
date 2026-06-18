@echo off
setlocal
title Psycho-Core - Import All SQL
cd /d "%~dp0"

echo ===========================================================================
echo   Psycho-Core 8.3.7 - Full Database Import
echo ===========================================================================
echo.
echo This script imports ALL SQL into the correct databases:
echo   sql\base\1_auth.sql         -> psycho_auth
echo   sql\base\2_characters.sql   -> psycho_characters
echo   sql\updates\*               -> psycho_world / psycho_characters / psycho_hotfixes
echo   modules\mod-psychobot\sql   -> psycho_auth / psycho_characters / psycho_world
echo.
echo You will be asked for the MariaDB root password for each import.
echo.

if not exist "..\..\bin\mysql.exe" (
  if not exist "..\dep\mysql\bin\mysql.exe" (
    echo ERROR: Could not find mysql.exe.
    echo Run this from the server root, or ensure dep\mysql\bin\mysql.exe exists.
    pause
    exit /b 1
  )
  set "MYSQL=..\dep\mysql\bin\mysql.exe"
  set "INI=..\dep\mysql\data\my.ini"
) else (
  set "MYSQL=..\..\bin\mysql.exe"
  set "INI=..\..\bin\my.ini"
)

echo === Step 1: Create databases + user ===
"..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p < "..\dep\mysql\create_psycho_databases.sql"
if errorlevel 1 (
  echo ERROR: Database creation failed.
  pause
  exit /b 1
)
echo Done.
echo.

echo === Step 2: Import auth base ===
"..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p psycho_auth < "..\sql\base\1_auth.sql"
echo Done.
echo.

echo === Step 3: Import characters base ===
"..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p psycho_characters < "..\sql\base\2_characters.sql"
echo Done.
echo.

echo === Step 4: Import world updates ===
for %%f in ("..\sql\updates\world\*.sql") do (
  echo   Importing %%~nxf...
  "..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p psycho_world < "%%f"
)
echo Done.
echo.

echo === Step 5: Import characters updates ===
for %%f in ("..\sql\updates\characters\*.sql") do (
  echo   Importing %%~nxf...
  "..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p psycho_characters < "%%f"
)
echo Done.
echo.

echo === Step 6: Import hotfixes updates ===
for %%f in ("..\sql\updates\hotfixes\*.sql") do (
  echo   Importing %%~nxf...
  "..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p psycho_hotfixes < "%%f"
)
echo Done.
echo.

echo === Step 7: Import mod-psychobot SQL ===
echo   Auth...
"..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p psycho_auth < "..\modules\mod-psychobot\sql\auth\psychobot_rbac.sql"
echo   Characters...
for %%f in ("..\modules\mod-psychobot\sql\characters\*.sql") do (
  echo     %%~nxf...
  "..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p psycho_characters < "%%f"
)
echo   World...
for %%f in ("..\modules\mod-psychobot\sql\world\*.sql") do (
  echo     %%~nxf...
  "..\dep\mysql\bin\mysql.exe" --defaults-file=..\dep\mysql\data\my.ini -uroot -p psycho_world < "%%f"
)
echo Done.
echo.

echo ===========================================================================
echo   All SQL imported successfully!
echo   Databases: psycho_auth, psycho_characters, psycho_world, psycho_hotfixes
echo   User: psycho / core
echo ===========================================================================
echo.
pause
