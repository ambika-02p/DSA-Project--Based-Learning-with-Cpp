@echo off
cd /d "%~dp0"
g++ src\main.cpp -std=c++11 -o CampusNavX.exe
if errorlevel 1 (
  echo Build failed. Check that MinGW g++ is installed and added to PATH.
  pause
  exit /b 1
)
CampusNavX.exe
pause
