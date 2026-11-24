#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include "../DataStructures/Graph.h"

using namespace std;

class Dijkstra {
public:
    static vector<int> findRoute(Graph &g, int start,
                                 int goal, bool accessible) {
        int n = g.size();

        vector<int> dist(n + 1, INT_MAX);
        vector<int> parent(n + 1, -1);

        priority_queue<
            pair<int, int>,
            vector<pair<int, int> >,
            greater<pair<int, int> >
        > pq;

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

            vector<Edge> edges = g.getNeighbours(u);

            for (int i = 0; i < (int)edges.size(); i++) {
                Edge e = edges[i];
                int v = e.destination;

                if (e.blocked)
                    continue;

                if (accessible && e.stairs)
                    continue;

                if (dist[u] != INT_MAX &&
                    dist[u] + e.distance < dist[v]) {
                    dist[v] = dist[u] + e.distance;
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

    static int getDistance(Graph &g, vector<int> path) {
        int total = 0;

        for (int i = 0; i + 1 < (int)path.size(); i++) {
            vector<Edge> edges = g.getNeighbours(path[i]);

            for (int j = 0; j < (int)edges.size(); j++) {
                if (edges[j].destination == path[i + 1]) {
                    total += edges[j].distance;
                    break;
                }
            }
        }

        return total;
    }
};

#endif
