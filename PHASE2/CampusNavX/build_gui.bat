@echo off
setlocal
cd /d "%~dp0"
echo Building CampusNavX GUI communication server...
g++ -std=c++11 -O2 -I. server.cpp "Data Structures\Graph.cpp" "Data Structures\IndoorGraph.cpp" algorithms\BFS.cpp algorithms\DFS.cpp algorithms\Dijkstra.cpp algorithms\MinHeap.cpp "File Handling\FileManager.cpp" navigation\Navigation.cpp navigation\IndoorNavigation.cpp navigation\AccessibleNavigation.cpp navigation\EmergencyNavigation.cpp -o CampusNavX_GUI_Server.exe -lws2_32
if errorlevel 1 (
  echo.
  echo GUI server build failed. Check the compiler errors above.
  pause
  exit /b 1
)
echo GUI server build successful.
endlocal
