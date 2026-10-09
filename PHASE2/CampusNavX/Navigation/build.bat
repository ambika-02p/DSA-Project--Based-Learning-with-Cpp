@echo off
g++ -std=c++11 -I. main.cpp "Data Structures\Graph.cpp" "Data Structures\IndoorGraph.cpp" algorithms\BFS.cpp algorithms\DFS.cpp algorithms\Dijkstra.cpp algorithms\MinHeap.cpp "File Handling\FileManager.cpp" navigation\Navigation.cpp navigation\IndoorNavigation.cpp navigation\AccessibleNavigation.cpp navigation\EmergencyNavigation.cpp -o CampusNavX.exe
if errorlevel 1 (
  echo Build failed. Check the compiler errors above.
  pause
  exit /b 1
)
echo Build successful.
pause
