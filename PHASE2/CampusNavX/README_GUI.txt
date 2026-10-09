CampusNavX GUI - Setup and Run
================================

WHAT IS INCLUDED
- The original C++ console backend and data files are preserved.
- frontend/index.html, frontend/style.css and frontend/app.js provide the browser interface.
- server.cpp is a small localhost HTTP bridge that calls the existing C++ graph, BFS, DFS, Dijkstra and file-handling code.
- The bridge binds only to 127.0.0.1 on port 8765. It is intended for local project/demo use.

REQUIREMENTS
- Windows with MinGW g++ available in PowerShell/Command Prompt PATH.
- Keep the complete folder structure. Do not move the frontend or data folders out of the project directory.

RUN THE GUI
1. Extract the complete project folder.
2. Open the CampusNavX_Backend folder in File Explorer.
3. Double-click run_gui.bat.
4. A server console and your browser should open. If needed, browse to http://127.0.0.1:8765/
5. Keep the server console open while using the GUI. Close that console to stop the GUI server.

BUILD THE GUI SERVER ONLY
- Double-click build_gui.bat, or run it from PowerShell: .\build_gui.bat

RUN FROM POWERSHELL
Set-Location "$HOME\OneDrive\Documents\CampusNavX_Backend"
.\run_gui.bat
(Adjust the path if you extracted the folder somewhere else.)

GUI FEATURES
- Outdoor and indoor location selectors
- Accessible BFS, DFS, Dijkstra and comparison view for outdoor routes
- Dijkstra indoor route planning
- Distance, estimated walking time and step-by-step location sequence
- Searchable campus directory and schematic location overview
- Block/unblock pathway controls with CSV persistence
- Route history

IMPORTANT NOTES
- Accessibility filtering is enforced by the existing backend algorithms: blocked links and stairs-only links are skipped.
- The campus overview is a schematic layout for demonstration, not a geographically accurate map.
- Outdoor BFS finds a path with fewest edges; DFS shows a depth-first exploration path; Dijkstra computes the weighted shortest-distance path.
- Existing main.cpp and the original console build are not replaced. Use build.bat for the original console application.
- Route history is stored in data/route_history.txt.
- If Windows Firewall asks, the server should only be used locally; it binds to loopback (127.0.0.1).
