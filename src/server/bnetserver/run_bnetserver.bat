@echo off
title Psycho-Core Bnetserver
cd /d "%~dp0"
echo ============================================
echo   Psycho-Core 8.3.7 - Bnetserver
echo ============================================
echo.
start /wait bnetserver.exe
echo.
echo ============================================
echo   bnetserver.exe has stopped.
echo   Press any key to close this window.
echo ============================================
pause >nul
