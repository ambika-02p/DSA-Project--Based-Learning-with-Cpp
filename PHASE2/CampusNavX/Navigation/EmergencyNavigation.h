#ifndef EMERGENCY_NAVIGATION_H
#define EMERGENCY_NAVIGATION_H
#include "../Data Structures/Graph.h"
#include "../algorithms/Dijkstra.h"
class EmergencyNavigation {
public:
 static RouteResult nearestAccessibleExit(const Graph&,int source);
};
#endif
