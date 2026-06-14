@echo off
setlocal
cd /d "%~dp0bin"

if not exist "mysqld.exe" (
  echo ERROR: mysqld.exe was not found in this bin folder.
  echo Extract/copy the MariaDB package contents so mysql\bin\mysqld.exe exists.
  pause
  exit /b 1
)

if not exist "..\data" mkdir "..\data"
if not exist "..\logs" mkdir "..\logs"
if not exist "..\tmp" mkdir "..\tmp"

if not exist "..\data\my.ini" (
  echo data\my.ini was missing. Copying Psycho_Core portable my.ini...
  copy /Y "..\my.ini" "..\data\my.ini" >nul
) else (
  findstr /C:"Psycho_Core portable MariaDB/MySQL config" "..\data\my.ini" >nul 2>nul
  if errorlevel 1 (
    echo Replacing generated data\my.ini with Psycho_Core portable my.ini...
    copy /Y "..\my.ini" "..\data\my.ini" >nul
  )
)

echo Starting portable MariaDB from: %CD%
echo Using active config: ..\data\my.ini
echo Press CTRL+C in this window to stop, or use ..\stop_mysql.bat from another window.

REM Runs from mysql\bin and uses the active portable config in mysql\data\my.ini.
mysqld.exe --defaults-file=../data/my.ini --console
