
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "Graph.h"
#include "IndoorGraph.h"

using namespace std;

class FileManager {
private:

    bool isNumber(string s) {
        if (s.empty())
            return false;

        for (int i = 0; i < (int)s.length(); i++) {
            if (s[i] < '0' || s[i] > '9')
                return false;
        }

        return true;
    }

public:

    bool loadLocations(string filename, Graph &graph) {
        ifstream file(filename.c_str());

        if (!file.is_open()) {
            cout << "Cannot open " << filename << "\n";
            return false;
        }

        string line;
        getline(file, line);

        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, type;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, type, ',');

            if (!name.empty())
                graph.addLocation(name);
        }

        return true;
    }

    bool loadPathways(string filename, Graph &graph) {
        ifstream file(filename.c_str());

        if (!file.is_open()) {
            cout << "Cannot open " << filename << "\n";
            return false;
        }

        string line;
        getline(file, line);

        while (getline(file, line)) {
            stringstream ss(line);

            string source, destination;
            string distance, time, blocked, stairs;

            getline(ss, source, ',');
            getline(ss, destination, ',');
            getline(ss, distance, ',');
            getline(ss, time, ',');
            getline(ss, blocked, ',');
            getline(ss, stairs, ',');

            if (!isNumber(source) || !isNumber(destination) ||
                !isNumber(distance) || !isNumber(time) ||
                !isNumber(blocked) || !isNumber(stairs))
                continue;

            int u = atoi(source.c_str());
            int v = atoi(destination.c_str());

            if (u <= 0 || v <= 0 ||
                u > graph.size() || v > graph.size())
                continue;

            graph.addPathway(
                u, v,
                atoi(distance.c_str()),
                atoi(time.c_str()),
                atoi(stairs.c_str()) == 1
            );

            if (atoi(blocked.c_str()) == 1)
                graph.blockPathway(u, v, true);
        }

        return true;
    }

    bool loadIndoorLocations(string filename, IndoorGraph &graph) {
        ifstream file(filename.c_str());

        if (!file.is_open()) {
            cout << "Cannot open " << filename << "\n";
            return false;
        }

        string line;
        getline(file, line);

        while (getline(file, line)) {
            stringstream ss(line);

            string id, name, building, floor, type;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, building, ',');
            getline(ss, floor, ',');
            getline(ss, type, ',');

            if (name.empty() || building.empty() ||
                !isNumber(floor))
                continue;

            graph.addLocation(
                name, building, atoi(floor.c_str()), type
            );
        }

        return true;
    }

    bool loadIndoorPathways(string filename, IndoorGraph &graph) {
        ifstream file(filename.c_str());

        if (!file.is_open()) {
            cout << "Cannot open " << filename << "\n";
            return false;
        }

        string line;
        getline(file, line);

        while (getline(file, line)) {
            stringstream ss(line);

            string source, destination;
            string distance, time, blocked, stairs;

            getline(ss, source, ',');
            getline(ss, destination, ',');
            getline(ss, distance, ',');
            getline(ss, time, ',');
            getline(ss, blocked, ',');
            getline(ss, stairs, ',');

            if (!isNumber(source) || !isNumber(destination) ||
                !isNumber(distance) || !isNumber(time) ||
                !isNumber(blocked) || !isNumber(stairs))
                continue;

            int u = atoi(source.c_str());
            int v = atoi(destination.c_str());

            if (u <= 0 || v <= 0 ||
                u > graph.size() || v > graph.size())
                continue;

            graph.addPathway(
                u, v,
                atoi(distance.c_str()),
                atoi(time.c_str()),
                atoi(stairs.c_str()) == 1
            );

            if (atoi(blocked.c_str()) == 1)
                graph.blockPathway(u, v, true);
        }

        return true;
    }
};

#endif
