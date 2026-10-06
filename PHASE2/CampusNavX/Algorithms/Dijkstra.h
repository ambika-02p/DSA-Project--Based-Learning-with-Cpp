#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "../Data Structures/Graph.h"
#include "../Data Structures/MinHeap.h"
#include "../Data Structures/LinkedList.h"


class Dijkstra{
    public:
        static void findShortestPath(Graph& graph, int source, int destination);
};

#endif