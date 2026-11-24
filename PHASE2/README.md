# CampusNavX (starter)

CampusNavX is an offline-first campus navigation project using a local schematic map and a C++ graph backend.

## Run the UI
1. Open the `CampusNavX` folder.
2. Double-click `frontend/index.html` or open it in a browser.
3. No internet connection is required for the prototype.

## Run the C++ backend (Windows + MinGW)
1. Ensure `g++` is installed and available in PATH.
2. Double-click `build.bat`, or run from PowerShell in this folder:
   `g++ src/main.cpp -std=c++11 -o CampusNavX.exe`
   `./CampusNavX.exe`

## Included now
- Navy blue and orange dashboard
- Sidebar navigation: Navigate, Locations, Pathway Control, Route History
- Offline schematic campus map with highlighted routes
- Shortest available route using Dijkstra in the browser prototype
- Block/unblock pathway simulation
- Accessible-path preference in the sample graph
- Route history stored locally by the browser
- C++ starter graph and Dijkstra implementation
- CSV sample data

## Important current limitation
The HTML/JavaScript UI and C++ executable are separate prototypes at this stage. The UI calculates routes in JavaScript using the sample graph; it does not yet call the C++ executable. Indoor room/floor navigation and a live C++/UI bridge are planned integration steps. Sample map distances and locations are illustrative, not verified campus measurements.
