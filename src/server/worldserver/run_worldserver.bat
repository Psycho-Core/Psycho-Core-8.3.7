@echo off
title Psycho-Core Worldserver
cd /d "%~dp0"
echo ============================================
echo   Psycho-Core 8.3.7 - Worldserver
echo ============================================
echo.
start /wait worldserver.exe
echo.
echo ============================================
echo   worldserver.exe has stopped.
echo   Press any key to close this window.
echo ============================================
pause >nul
