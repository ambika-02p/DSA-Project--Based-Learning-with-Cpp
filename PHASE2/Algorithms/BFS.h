
#ifndef BFS_H
#define BFS_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "../DataStructures/Graph.h"

using namespace std;

class BFS {
public:
    static vector<int> findRoute(Graph &g, int start,
                                 int goal, bool accessible) {
        vector<int> parent(g.size() + 1, -1);
        vector<bool> visited(g.size() + 1, false);
        queue<int> q;

        q.push(start);
        visited[start] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            if (u == goal)
                break;

            vector<Edge> edges = g.getNeighbours(u);

            for (int i = 0; i < (int)edges.size(); i++) {
                int v = edges[i].destination;

                if (edges[i].blocked)
                    continue;

                if (accessible && edges[i].stairs)
                    continue;

                if (!visited[v]) {
                    visited[v] = true;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }

        vector<int> path;

        if (!visited[goal])
            return path;

        for (int v = goal; v != -1; v = parent[v])
            path.push_back(v);

        reverse(path.begin(), path.end());
        return path;
    }
};

#endif
