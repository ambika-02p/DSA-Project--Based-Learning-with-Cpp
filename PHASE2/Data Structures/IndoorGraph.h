
#ifndef INDOOR_GRAPH_H
#define INDOOR_GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <queue>
#include <climits>
#include "../DataStructures/IndoorEdge.h"

using namespace std;

class IndoorGraph {
private:
    vector<string> names;
    vector<string> buildings;
    vector<string> types;
    vector<int> floors;
    vector<vector<IndoorEdge> > adj;

    string lowerCase(string s) {
        transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) {
                return (char)tolower(c);
            });
        return s;
    }

public:
    IndoorGraph() {
        names.push_back("");
        buildings.push_back("");
        types.push_back("");
        floors.push_back(0);
        adj.push_back(vector<IndoorEdge>());
    }

    int addLocation(string name, string building,
                    int floor, string type) {
        names.push_back(name);
        buildings.push_back(building);
        floors.push_back(floor);
        types.push_back(type);
        adj.push_back(vector<IndoorEdge>());

        return (int)names.size() - 1;
    }

    int findLocationId(string name) {
        name = lowerCase(name);

        for (int i = 1; i < (int)names.size(); i++) {
            if (lowerCase(names[i]) == name)
                return i;
        }

        return -1;
    }

    string getLocationName(int id) {
        if (id <= 0 || id >= (int)names.size())
            return "Unknown";

        return names[id];
    }

    string getBuilding(int id) {
        if (id <= 0 || id >= (int)buildings.size())
            return "";

        return buildings[id];
    }

    int getFloor(int id) {
        if (id <= 0 || id >= (int)floors.size())
            return -1;

        return floors[id];
    }

    int size() {
        return (int)names.size() - 1;
    }

    void addPathway(int u, int v, int distance,
                    int time, bool stairs) {
        if (u <= 0 || v <= 0 ||
            u >= (int)adj.size() ||
            v >= (int)adj.size())
            return;

        adj[u].push_back(IndoorEdge(v, distance, time, stairs));
        adj[v].push_back(IndoorEdge(u, distance, time, stairs));
    }

    vector<IndoorEdge> getNeighbours(int id) {
        if (id <= 0 || id >= (int)adj.size())
            return vector<IndoorEdge>();

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

    vector<int> shortestPath(int start, int goal,
                             bool accessible) {
        int n = size();

        vector<int> dist(n + 1, INT_MAX);
        vector<int> parent(n + 1, -1);

        priority_queue<
            pair<int, int>,
            vector<pair<int, int> >,
            greater<pair<int, int> >
        > pq;

        if (start <= 0 || goal <= 0 ||
            start > n || goal > n)
            return vector<int>();

        dist[start] = 0;
        pq.push(make_pair(0, start));

        while (!pq.empty()) {
            int cost = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (cost != dist[u])
                continue;

            if (u == goal)
                break;

            for (int i = 0; i < (int)adj[u].size(); i++) {
                IndoorEdge e = adj[u][i];
                int v = e.destination;

                if (e.blocked)
                    continue;

                if (accessible && e.stairs)
                    continue;

                if (cost + e.distance < dist[v]) {
                    dist[v] = cost + e.distance;
                    parent[v] = u;
                    pq.push(make_pair(dist[v], v));
                }
            }
        }

        vector<int> path;

        if (dist[goal] == INT_MAX)
            return path;

        for (int v = goal; v != -1; v = parent[v])
            path.push_back(v);

        reverse(path.begin(), path.end());
        return path;
    }

    void showLocations() {
        cout << "\n--- Indoor Locations ---\n";

        for (int i = 1; i < (int)names.size(); i++) {
            cout << i << ". " << names[i]
                 << " | " << buildings[i]
                 << " | Floor " << floors[i] << "\n";
        }
    }
};

#endif
