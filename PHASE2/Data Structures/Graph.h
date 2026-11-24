#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include "Edge.h"

using namespace std;

class Graph {
private:
    vector<string> locations;
    vector<vector<Edge> > adj;

    string lowerCase(string s) {
        transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) {
                return (char)tolower(c);
            });
        return s;
    }

public:
    Graph() {
        locations.push_back("");
        adj.push_back(vector<Edge>());
    }

    int addLocation(string name) {
        int oldId = findLocationId(name);

        if (oldId != -1)
            return oldId;

        locations.push_back(name);
        adj.push_back(vector<Edge>());

        return (int)locations.size() - 1;
    }

    int findLocationId(string name) {
        name = lowerCase(name);

        for (int i = 1; i < (int)locations.size(); i++) {
            if (lowerCase(locations[i]) == name)
                return i;
        }

        return -1;
    }

    string getLocationName(int id) {
        if (id <= 0 || id >= (int)locations.size())
            return "Unknown";

        return locations[id];
    }

    int size() {
        return (int)locations.size() - 1;
    }

    void addPathway(int u, int v, int distance,
                    int time, bool stairs) {
        if (u <= 0 || v <= 0 ||
            u >= (int)adj.size() ||
            v >= (int)adj.size())
            return;

        adj[u].push_back(Edge(v, distance, time, stairs));
        adj[v].push_back(Edge(u, distance, time, stairs));
    }

    vector<Edge> getNeighbours(int id) {
        if (id <= 0 || id >= (int)adj.size())
            return vector<Edge>();

        return adj[id];
    }

    void blockPathway(int u, int v, bool status) {
        if (u <= 0 || v <= 0 ||
            u >= (int)adj.size() ||
            v >= (int)adj.size())
            return;

        for (int i = 0; i < (int)adj[u].size(); i++) {
            if (adj[u][i].destination == v)
                adj[u][i].blocked = status;
        }

        for (int i = 0; i < (int)adj[v].size(); i++) {
            if (adj[v][i].destination == u)
                adj[v][i].blocked = status;
        }
    }

    void showLocations() {
        cout << "\n--- Campus Locations ---\n";

        for (int i = 1; i < (int)locations.size(); i++)
            cout << i << ". " << locations[i] << "\n";
    }
};

#endif
