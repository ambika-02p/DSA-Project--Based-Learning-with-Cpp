
#ifndef INDOOR_EDGE_H
#define INDOOR_EDGE_H

class IndoorEdge {
public:
    int destination;
    int distance;
    int travelTime;
    bool blocked;
    bool stairs;

    IndoorEdge(int dest, int dist, int time = 1, bool hasStairs = false) {
        destination = dest;
        distance = dist;
        travelTime = time;
        blocked = false;
        stairs = hasStairs;
    }
};

#endif
