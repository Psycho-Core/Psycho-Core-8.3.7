@echo off
setlocal
cd /d "%~dp0bin"

if not exist "mysql.exe" (
  echo ERROR: mysql.exe was not found in this bin folder.
  echo Extract/copy the MariaDB package contents so mysql\bin\mysql.exe exists.
  pause
  exit /b 1
)

if not exist "..\data\my.ini" copy /Y "..\my.ini" "..\data\my.ini" >nul

mysql.exe --defaults-file=../data/my.ini -uroot -p
