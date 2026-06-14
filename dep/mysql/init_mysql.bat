@echo off
setlocal
cd /d "%~dp0bin"

if exist "..\data\mysql" (
  echo The data folder already looks initialized: %~dp0data
  echo Applying Psycho_Core portable config to data\my.ini...
  copy /Y "..\my.ini" "..\data\my.ini" >nul
  echo Done. Active config is now: %~dp0data\my.ini
  pause
  exit /b 0
)

if not exist "..\data" mkdir "..\data"
if not exist "..\logs" mkdir "..\logs"
if not exist "..\tmp" mkdir "..\tmp"

set /p MYSQL_ROOT_PASSWORD=Enter NEW MariaDB root password:
if "%MYSQL_ROOT_PASSWORD%"=="" (
  echo ERROR: password cannot be empty.
  pause
  exit /b 1
)

if exist "mariadb-install-db.exe" (
  mariadb-install-db.exe --datadir=../data --password=%MYSQL_ROOT_PASSWORD%
  if errorlevel 1 (
    echo ERROR: mariadb-install-db.exe failed.
    pause
    exit /b 1
  )
  echo Replacing generated data\my.ini with Psycho_Core portable my.ini...
  copy /Y "..\my.ini" "..\data\my.ini" >nul
  echo Done. Active config is now: %~dp0data\my.ini
  pause
  exit /b 0
)

if exist "mysql_install_db.exe" (
  mysql_install_db.exe --datadir=../data --password=%MYSQL_ROOT_PASSWORD%
  if errorlevel 1 (
    echo ERROR: mysql_install_db.exe failed.
    pause
    exit /b 1
  )
  echo Replacing generated data\my.ini with Psycho_Core portable my.ini...
  copy /Y "..\my.ini" "..\data\my.ini" >nul
  echo Done. Active config is now: %~dp0data\my.ini
  pause
  exit /b 0
)

echo ERROR: neither mariadb-install-db.exe nor mysql_install_db.exe was found in mysql\bin.
echo Extract/copy a MariaDB Windows package that includes server initialization tools.
pause
exit /b 1
