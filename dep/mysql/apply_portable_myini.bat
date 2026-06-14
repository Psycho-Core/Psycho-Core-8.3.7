@echo off
setlocal
cd /d "%~dp0"

if not exist "data" mkdir "data"

if not exist "my.ini" (
  echo ERROR: root my.ini was not found in this mysql folder.
  pause
  exit /b 1
)

echo Replacing data\my.ini with Psycho_Core portable my.ini...
copy /Y "my.ini" "data\my.ini" >nul
echo Done. Active config is now: %CD%\data\my.ini
pause
