#ifndef INDOORBFS_H
#define INDOORBFS_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include "../DataStructures/IndoorGraph.h"

using namespace std;

class IndoorBFS {
public:

    static vector<int> findPath(IndoorGraph &g, int src,
                                int dest, bool accessible) {

        int n = g.size();

        // Validate source and destination
        if (src <= 0 || dest <= 0 || src > n || dest > n) {
            return vector<int>();
        }

        vector<int> parent(n + 1, -1);
        vector<bool> visited(n + 1, false);

        queue<int> q;

        q.push(src);
        visited[src] = true;

        while (!q.empty()) {

            int curr = q.front();
            q.pop();

            if (curr == dest) {
                break;
            }

            vector<IndoorEdge> edges = g.getNeighbours(curr);

            for (int i = 0; i < (int)edges.size(); i++) {

                int next = edges[i].destination;

                // Ignore invalid neighbours
                if (next <= 0 || next > n) {
                    continue;
                }

                // Skip blocked pathways
                if (edges[i].blocked) {
                    continue;
                }

                // Skip stairs in accessible mode
                if (accessible && edges[i].stairs) {
                    continue;
                }

                if (!visited[next]) {

                    visited[next] = true;
                    parent[next] = curr;

                    q.push(next);
                }
            }
        }

        // Reconstruct path
        vector<int> path;

        if (!visited[dest]) {
            return path;
        }

        for (int curr = dest; curr != -1; curr = parent[curr]) {
            path.push_back(curr);
        }

        reverse(path.begin(), path.end());

        return path;
    }
};

#endif