
#ifndef PATHWAY_CONTROL_H
#define PATHWAY_CONTROL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Graph.h"
#include "IndoorGraph.h"

using namespace std;

class PathwayControl {
public:

    bool saveOutdoor(Graph &graph, string filename) {
        ofstream file(filename.c_str());

        if (!file.is_open()) {
            cout << "Cannot save outdoor status.\n";
            return false;
        }

        file << "source,destination,blocked\n";

        for (int u = 1; u < graph.size(); u++) {
            vector<Edge> edges = graph.getNeighbours(u);

            for (int i = 0; i < (int)edges.size(); i++) {
                int v = edges[i].destination;

                if (u < v) {
                    file << u << "," << v << ","
                         << edges[i].blocked << "\n";
                }
            }
        }

        file.close();
        return true;
    }

    bool loadOutdoor(Graph &graph, string filename) {
        ifstream file(filename.c_str());

        // No saved file means use the original CSV defaults.
        if (!file.is_open()) {
            return true;
        }

        string line;
        getline(file, line);

        while (getline(file, line)) {
            stringstream row(line);
            int u, v, blocked;
            char comma1, comma2;

            if (row >> u >> comma1 >> v
                    >> comma2 >> blocked) {
                graph.blockPathway(u, v, blocked == 1);
            }
        }

        file.close();
        return true;
    }

    bool saveIndoor(IndoorGraph &graph, string filename) {
        ofstream file(filename.c_str());

        if (!file.is_open()) {
            cout << "Cannot save indoor status.\n";
            return false;
        }

        file << "source,destination,blocked\n";

        for (int u = 1; u < graph.size(); u++) {
            vector<IndoorEdge> edges = graph.getNeighbours(u);

            for (int i = 0; i < (int)edges.size(); i++) {
                int v = edges[i].destination;

                if (u < v) {
                    file << u << "," << v << ","
                         << edges[i].blocked << "\n";
                }
            }
        }

        file.close();
        return true;
    }

    bool loadIndoor(IndoorGraph &graph, string filename) {
        ifstream file(filename.c_str());

        if (!file.is_open()) {
            return true;
        }

        string line;
        getline(file, line);

        while (getline(file, line)) {
            stringstream row(line);
            int u, v, blocked;
            char comma1, comma2;

            if (row >> u >> comma1 >> v
                    >> comma2 >> blocked) {
                graph.blockPathway(u, v, blocked == 1);
            }
        }

        file.close();
        return true;
    }
};

#endif
