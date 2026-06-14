@echo off
setlocal
cd /d "%~dp0bin"

if not exist "mysqladmin.exe" (
  echo ERROR: mysqladmin.exe was not found in this bin folder.
  echo Extract/copy the MariaDB package contents so mysql\bin\mysqladmin.exe exists.
  pause
  exit /b 1
)

if not exist "..\data\my.ini" copy /Y "..\my.ini" "..\data\my.ini" >nul

echo Stopping portable MariaDB from: %CD%
mysqladmin.exe --defaults-file=../data/my.ini -uroot -p shutdown
pause
