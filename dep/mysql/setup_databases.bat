@echo off
setlocal enabledelayedexpansion
title Psycho-Core - Database Setup
cd /d "%~dp0"

echo ===========================================================================
echo   Psycho-Core 8.3.7 - Database Setup (One-Click)
echo ===========================================================================
echo.
echo This imports ALL databases in the correct order.
echo You will be asked for the MariaDB root password for each step.
echo.
echo Press any key to start, or close this window to cancel.
pause >nul

set "MYSQL=bin\mysql.exe"
set "INI=data\my.ini"

if not exist "%MYSQL%" (
  echo ERROR: mysql.exe not found at %MYSQL%
  echo Extract the MariaDB package so dep\mysql\bin\mysql.exe exists.
  pause
  exit /b 1
)

if not exist "%INI%" copy /Y "my.ini" "%INI%" >nul 2>nul

set "ROOT=..\.."

echo.
echo === Step 1: Create databases and user ===
"%MYSQL%" --defaults-file=%INI% -uroot -p < "create_psycho_databases.sql"
if errorlevel 1 ( echo ERROR: Could not create databases. & pause & exit /b 1 )
echo Done.
echo.

echo === Step 2: Import AUTH database ===
"%MYSQL%" --defaults-file=%INI% -uroot -p psycho_auth < "%ROOT%\sql\base\auth_database.sql"
if errorlevel 1 ( echo ERROR: Could not import auth database. & pause & exit /b 1 )
echo Done.
echo.

echo === Step 3: Import CHARACTERS database (updates merged in) ===
"%MYSQL%" --defaults-file=%INI% -uroot -p psycho_characters < "%ROOT%\sql\base\characters_database.sql"
if errorlevel 1 ( echo ERROR: Could not import characters database. & pause & exit /b 1 )
echo Done.
echo.

echo === Step 4: Import WORLD database ===
if exist "%ROOT%\sql\base\world_database.sql" (
  echo Importing ^(this is the big file, may take a few minutes^)...
  "%MYSQL%" --defaults-file=%INI% -uroot -p psycho_world < "%ROOT%\sql\base\world_database.sql"
  if errorlevel 1 ( echo ERROR: Could not import world database. & pause & exit /b 1 )
  echo Done.
) else (
  echo SKIP: world_database.sql not found in sql\base\
  echo Download DB-1 to DB-4 from GitHub Releases, combine, and place here.
)
echo.

echo === Step 5: Apply WORLD updates (all 28 merged) ===
"%MYSQL%" --defaults-file=%INI% -uroot -p psycho_world < "%ROOT%\sql\base\world_updates.sql"
echo Done.
echo.

echo === Step 6: Import HOTFIXES database ===
if exist "%ROOT%\sql\base\hotfixes_database.sql" (
  "%MYSQL%" --defaults-file=%INI% -uroot -p psycho_hotfixes < "%ROOT%\sql\base\hotfixes_database.sql"
  if errorlevel 1 ( echo ERROR: Could not import hotfixes database. & pause & exit /b 1 )
  echo Done.
) else (
  echo SKIP: hotfixes_database.sql not found in sql\base\
)
echo.

echo === Step 7: Apply HOTFIXES updates ===
"%MYSQL%" --defaults-file=%INI% -uroot -p psycho_hotfixes < "%ROOT%\sql\base\hotfixes_updates.sql"
echo Done.
echo.

echo === Step 8: Import mod-psychobot SQL ===
echo   Auth...
"%MYSQL%" --defaults-file=%INI% -uroot -p psycho_auth < "%ROOT%\modules\mod-psychobot\sql\auth\psychobot_rbac.sql"
echo   Characters...
for %%f in ("%ROOT%\modules\mod-psychobot\sql\characters\*.sql") do (
  "%MYSQL%" --defaults-file=%INI% -uroot -p psycho_characters < "%%f"
)
echo   World...
for %%f in ("%ROOT%\modules\mod-psychobot\sql\world\*.sql") do (
  "%MYSQL%" --defaults-file=%INI% -uroot -p psycho_world < "%%f"
)
echo Done.
echo.

echo ===========================================================================
echo   ALL DATABASES IMPORTED SUCCESSFULLY!
echo.
echo   psycho_auth        - account/login data
echo   psycho_characters  - character data (updates included)
echo   psycho_world       - world data + 28 updates applied
echo   psycho_hotfixes    - hotfix data + updates applied
echo   mod-psychobot      - bot SQL applied
echo.
echo   User: psycho / psycho    Port: 3307
echo ===========================================================================
echo.
pause
