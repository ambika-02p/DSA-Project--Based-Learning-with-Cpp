#ifndef EDGE_H
#define EDGE_H

class Edge {
public:
    int destination;
    int distance;
    int travelTime;
    bool blocked;
    bool stairs;

    Edge(int dest, int dist, int time = 1, bool hasStairs = false) {
        destination = dest;
        distance = dist;
        travelTime = time;
        blocked = false;
        stairs = hasStairs;
    }
};

#endif

