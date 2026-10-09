CampusNavX - Complete Backend (C++11)
====================================

This folder preserves the original CampusNavX graph-based backend and updates its integration.

FEATURES
- Outdoor and indoor graphs
- Name-based input (case-insensitive), IDs are internal only
- BFS and DFS accessible route candidates
- Dijkstra independently computes shortest-distance accessible route
- Accessible routing: blocked links are rejected; stair-only links are rejected; stairs with a ramp/elevator alternative are allowed
- Indoor locations shown with building and floor
- Emergency nearest accessible exit search
- Block/unblock outdoor and indoor links, saved back to CSV
- Persistent route history in data/route_history.txt
- CSV loading and validation of missing files

BUILD (run PowerShell from this folder)
g++ -std=c++11 -I. main.cpp "Data Structures/Graph.cpp" "Data Structures/IndoorGraph.cpp" algorithms/BFS.cpp algorithms/DFS.cpp algorithms/Dijkstra.cpp algorithms/MinHeap.cpp "File Handling/FileManager.cpp" navigation/Navigation.cpp navigation/IndoorNavigation.cpp navigation/AccessibleNavigation.cpp navigation/EmergencyNavigation.cpp -o CampusNavX.exe

RUN
.\CampusNavX.exe

IMPORTANT
- Run the executable from this project folder so the relative data/ paths work.
- CSV data are prototype data and do not represent a verified, live campus map. Validate paths, distances, exits, and accessibility before real-world use.
- This is a command-line backend, not the GUI yet.
