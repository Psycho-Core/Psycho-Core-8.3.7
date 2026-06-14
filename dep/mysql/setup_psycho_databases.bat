@echo off
setlocal
cd /d "%~dp0bin"

if not exist "mysql.exe" (
  echo ERROR: mysql.exe was not found in this bin folder.
  echo Extract/copy the MariaDB package contents so mysql\bin\mysql.exe exists.
  pause
  exit /b 1
)

if not exist "..\create_psycho_databases.sql" (
  echo ERROR: ..\create_psycho_databases.sql was not found.
  pause
  exit /b 1
)

if not exist "..\data\my.ini" copy /Y "..\my.ini" "..\data\my.ini" >nul

echo This creates psycho_auth, psycho_characters, psycho_world, psycho_hotfixes and the psycho DB user.
echo Default DB user/password in the SQL: psycho / core
echo You will be asked for the MariaDB root password.
mysql.exe --defaults-file=../data/my.ini -uroot -p < "..\create_psycho_databases.sql"
pause
