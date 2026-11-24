
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "DataStructures/Graph.h"
#include "DataStructures/IndoorGraph.h"
#include "DataStructures/FileManager.h"
#include "DataStructures/UnifiedNavigation.h"
#include "DataStructures/PathwayControl.h"
#include "DataStructures/RouteHistory.h"
#include "algorithms/Dijkstra.h"

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cout << "No command received.";
        return 1;
    }

    Graph outdoor;
    IndoorGraph indoor;
    FileManager fm;
    UnifiedNavigation nav;
    PathwayControl pc;
    RouteHistory rh("data/route_history.txt");

    // Load campus data
    if (!fm.loadLocations("data/locations.csv", outdoor) ||
        !fm.loadPathways("data/pathways.csv", outdoor) ||
        !fm.loadIndoorLocations("data/indoor_locations.csv", indoor) ||
        !fm.loadIndoorPathways("data/indoor_pathways.csv", indoor) ||
        !nav.loadConnectors("data/building_connectors.csv")) {

        cout << "ERROR: Campus data loading failed.";
        return 1;
    }

    // Restore saved pathway settings
    pc.loadOutdoor(outdoor, "data/outdoor_pathway_status.csv");
    pc.loadIndoor(indoor, "data/indoor_pathway_status.csv");

    string command = argv[1];

    // COMMAND 1: FIND ROUTE
    if (command == "route") {

        if (argc < 5) {
            cout << "ERROR: Missing route details.";
            return 1;
        }

        string start = argv[2];
        string destination = argv[3];
        bool accessible = (string(argv[4]) == "1");

        int outdoorStart = outdoor.findLocationId(start);
        int outdoorEnd = outdoor.findLocationId(destination);

        // OUTDOOR ROUTE
        if (outdoorStart != -1 && outdoorEnd != -1) {

            vector<int> path = Dijkstra::findRoute(
                outdoor, outdoorStart, outdoorEnd, accessible
            );

            if (path.empty()) {
                cout << "No outdoor route available.";
            }
            else {
                cout << "\n===== OUTDOOR ROUTE =====\n";

                for (int i = 0; i < (int)path.size(); i++) {
                    cout << outdoor.getLocationName(path[i]);

                    if (i + 1 < (int)path.size())
                        cout << " -> ";
                }

                cout << "\nTotal Distance: "
                     << Dijkstra::getDistance(outdoor, path);

                cout << "\nRoute Type: "
                     << (accessible ? "Accessible" : "Standard");

                cout << "\n=========================\n";

                rh.addRoute(
                    start,
                    destination,
                    accessible ? "Accessible outdoor route"
                               : "Standard outdoor route"
                );
            }
        }

        // INDOOR OR CROSS-BUILDING ROUTE
        else {

            ostringstream result;
            streambuf* oldBuffer = cout.rdbuf(result.rdbuf());

            bool found = nav.findUnifiedRoute(
                outdoor,
                indoor,
                start,
                destination,
                accessible
            );

            cout.rdbuf(oldBuffer);

            cout << result.str();

            if (found) {
                rh.addRoute(
                    start,
                    destination,
                    accessible ? "Accessible indoor route"
                               : "Standard indoor route"
                );
            }
        }
    }

    // COMMAND 2: SHOW LOCATIONS
    else if (command == "locations") {

        if (argc < 3) {
            cout << "ERROR: Specify indoor or outdoor.";
            return 1;
        }

        string type = argv[2];

        if (type == "indoor")
            indoor.showLocations();

        else if (type == "outdoor")
            outdoor.showLocations();

        else {
            cout << "ERROR: Invalid location type.";
            return 1;
        }
    }

    // COMMAND 3: BLOCK OR UNBLOCK PATHWAY
    else if (command == "pathway") {

        if (argc < 6) {
            cout << "ERROR: Missing pathway details.";
            return 1;
        }

        string type = argv[2];
        string action = argv[3];
        string first = argv[4];
        string second = argv[5];

        if (action != "block" && action != "unblock") {
            cout << "ERROR: Invalid pathway action.";
            return 1;
        }

        bool block = (action == "block");

        if (type == "outdoor") {

            int u = outdoor.findLocationId(first);
            int v = outdoor.findLocationId(second);

            if (u == -1 || v == -1) {
                cout << "ERROR: Outdoor location not found.";
                return 1;
            }

            outdoor.blockPathway(u, v, block);

            if (!pc.saveOutdoor(
                outdoor, "data/outdoor_pathway_status.csv")) {

                cout << "ERROR: Could not save pathway.";
                return 1;
            }
        }

        else if (type == "indoor") {

            int u = indoor.findLocationId(first);
            int v = indoor.findLocationId(second);

            if (u == -1 || v == -1) {
                cout << "ERROR: Indoor location not found.";
                return 1;
            }

            indoor.blockPathway(u, v, block);

            if (!pc.saveIndoor(
                indoor, "data/indoor_pathway_status.csv")) {

                cout << "ERROR: Could not save pathway.";
                return 1;
            }
        }

        else {
            cout << "ERROR: Invalid pathway type.";
            return 1;
        }

        cout << "Pathway successfully "
             << (block ? "blocked." : "unblocked.");
    }

    // COMMAND 4: SHOW ROUTE HISTORY
    else if (command == "history") {

        rh.showHistory();
    }

    else {
        cout << "ERROR: Unknown command.";
        return 1;
    }

    return 0;
}
