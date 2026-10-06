#ifndef DFS_H
#define DFS_H

#include "../Data Structures/Graph.h"

class DFS{
    private:
        static void dfs(Graph &graph, int current, bool visited[]);

    public:
        static void traverse(Graph &graph, int start);
};

#endif