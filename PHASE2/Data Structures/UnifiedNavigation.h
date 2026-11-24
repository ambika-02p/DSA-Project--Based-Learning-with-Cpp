#ifndef UNIFIED_NAVIGATION_H
#define UNIFIED_NAVIGATION_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Graph.h"
#include "IndoorGraph.h"
#include "../algorithms/Dijkstra.h"

using namespace std;

class UnifiedNavigation {

private:

    struct Connector {
        int outdoorId;
        int indoorId;
        string building;
    };

    vector<Connector> connectors;

    int findConnector(string building) {
        for (int i = 0; i < (int)connectors.size(); i++) {
            if (connectors[i].building == building)
                return i;
        }

        return -1;
    }

    void printIndoorPath(IndoorGraph &indoor,
                         vector<int> path) {

        for (int i = 0; i < (int)path.size(); i++) {
            cout << indoor.getLocationName(path[i]);

            if (i + 1 < (int)path.size())
                cout << " -> ";
        }

        cout << "\n";
    }

    void printOutdoorPath(Graph &outdoor,
                          vector<int> path) {

        for (int i = 0; i < (int)path.size(); i++) {
            cout << outdoor.getLocationName(path[i]);

            if (i + 1 < (int)path.size())
                cout << " -> ";
        }

        cout << "\n";
    }

public:

    bool loadConnectors(string filename) {

        connectors.clear();

        ifstream file(filename.c_str());

        if (!file.is_open())
            return false;

        string line;
        getline(file, line);

        while (getline(file, line)) {

            stringstream ss(line);

            string outdoorId, indoorId, building;

            getline(ss, outdoorId, ',');
            getline(ss, indoorId, ',');
            getline(ss, building, ',');

            if (outdoorId.empty() || indoorId.empty() ||
                building.empty())
                continue;

            Connector c;

            c.outdoorId = stoi(outdoorId);
            c.indoorId = stoi(indoorId);
            c.building = building;

            connectors.push_back(c);
        }

        return true;
    }

    bool findUnifiedRoute(Graph &outdoor,
                          IndoorGraph &indoor,
                          string startName,
                          string endName,
                          bool accessible) {

        int start = indoor.findLocationId(startName);
        int goal = indoor.findLocationId(endName);

        if (start == -1 || goal == -1) {
            cout << "Indoor location not found.\n";
            return false;
        }

        string startBuilding = indoor.getBuilding(start);
        string endBuilding = indoor.getBuilding(goal);

        int startConnector = findConnector(startBuilding);
        int endConnector = findConnector(endBuilding);

        if (startConnector == -1 || endConnector == -1) {
            cout << "Building connector not found.\n";
            return false;
        }

        Connector a = connectors[startConnector];
        Connector b = connectors[endConnector];

        // SAME BUILDING
        if (startBuilding == endBuilding) {

            vector<int> path =
                indoor.shortestPath(start, goal, accessible);

            if (path.empty()) {
                cout << "No indoor route available.\n";
                return false;
            }

            cout << "\n========== INDOOR ROUTE ==========\n";

            printIndoorPath(indoor, path);

            cout << "Route found successfully!\n";

            return true;
        }

        // START BUILDING
        vector<int> firstIndoor =
            indoor.shortestPath(start, a.indoorId, accessible);

        // BETWEEN BUILDINGS
        vector<int> outdoorPath =
            Dijkstra::findRoute(
                outdoor,
                a.outdoorId,
                b.outdoorId,
                accessible
            );

        // DESTINATION BUILDING
        vector<int> lastIndoor =
            indoor.shortestPath(b.indoorId, goal, accessible);

        if (firstIndoor.empty() ||
            outdoorPath.empty() ||
            lastIndoor.empty()) {

            cout << "No complete route found.\n";
            cout << "Check indoor pathways and building connectors.\n";

            return false;
        }

        cout << "\n========== COMPLETE ROUTE ==========\n";

        cout << "\n1. Inside " << startBuilding << ":\n";
        printIndoorPath(indoor, firstIndoor);

        cout << "\n2. Between buildings:\n";
        printOutdoorPath(outdoor, outdoorPath);

        cout << "\n3. Inside " << endBuilding << ":\n";
        printIndoorPath(indoor, lastIndoor);

        cout << "\nRoute found successfully!\n";

        cout << "Outdoor distance: "
             << Dijkstra::getDistance(outdoor, outdoorPath)
             << " metres\n";

        return true;
    }
};

#endif
