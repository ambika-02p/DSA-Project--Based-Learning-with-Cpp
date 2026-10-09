#ifndef ACCESSIBLE_NAVIGATION_H
#define ACCESSIBLE_NAVIGATION_H
#include "../Data Structures/Graph.h"
#include "../Data Structures/IndoorGraph.h"
#include "../algorithms/Dijkstra.h"
class AccessibleNavigation {
public:
 static RouteResult findRoute(const Graph&,int source,int destination);
 static RouteResult findIndoorRoute(const IndoorGraph&,int source,int destination);
};
#endif
