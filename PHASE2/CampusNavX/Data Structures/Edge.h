#ifndef EDGE_H
#define EDGE_H
struct Edge {
    int destination;
    double distance;
    double walkingTime;
    int blocked, stairs, elevator, ramp;
    Edge(int d=0, double dist=0, double time=0, int b=0,
         int s=0, int e=0, int r=0)
        : destination(d), distance(dist), walkingTime(time), blocked(b),
          stairs(s), elevator(e), ramp(r) {}
};
#endif
