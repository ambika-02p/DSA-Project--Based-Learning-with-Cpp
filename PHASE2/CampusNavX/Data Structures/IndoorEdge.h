#ifndef INDOOR_EDGE_H
#define INDOOR_EDGE_H
struct IndoorEdge {
    int destination;
    double distance;
    double walkingTime;
    int stairs, elevator, ramp, blocked;
    IndoorEdge(int d=0,double dist=0,double time=0,int s=0,int e=0,int r=0,int b=0)
      : destination(d),distance(dist),walkingTime(time),stairs(s),elevator(e),ramp(r),blocked(b) {}
};
#endif
