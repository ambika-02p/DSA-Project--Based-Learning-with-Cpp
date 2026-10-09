@echo off
setlocal
cd /d "%~dp0"
if not exist CampusNavX_GUI_Server.exe call build_gui.bat
if not exist CampusNavX_GUI_Server.exe exit /b 1
echo Starting CampusNavX GUI...
start "CampusNavX GUI Server" CampusNavX_GUI_Server.exe
timeout /t 2 /nobreak >nul
start "" http://127.0.0.1:8765/
echo.
echo The browser should open CampusNavX.
echo Keep the CampusNavX GUI Server window open while using the interface.
echo If the page does not open, visit http://127.0.0.1:8765/ manually.
endlocal
