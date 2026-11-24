
#ifndef DFS_H
#define DFS_H

#include <vector>
#include <algorithm>
#include "../DataStructures/Graph.h"

using namespace std;

class DFS {
private:
    static bool search(Graph &g, int u, int goal,
                       bool accessible,
                       vector<bool> &visited,
                       vector<int> &path) {
        visited[u] = true;
        path.push_back(u);

        if (u == goal)
            return true;

        vector<Edge> edges = g.getNeighbours(u);

        for (int i = 0; i < (int)edges.size(); i++) {
            int v = edges[i].destination;

            if (edges[i].blocked)
                continue;

            if (accessible && edges[i].stairs)
                continue;

            if (!visited[v]) {
                if (search(g, v, goal, accessible,
                           visited, path))
                    return true;
            }
        }

        path.pop_back();
        return false;
    }

public:
    static vector<int> findRoute(Graph &g, int start,
                                 int goal, bool accessible) {
        vector<bool> visited(g.size() + 1, false);
        vector<int> path;

        search(g, start, goal, accessible, visited, path);
        return path;
    }
};

#endif
