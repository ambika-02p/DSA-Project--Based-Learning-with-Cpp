#ifndef NAVIGATION_H
#define NAVIGATION_H
#include "../Data Structures/Graph.h"
#include "../algorithms/Dijkstra.h"
class Navigation {
public:
 static RouteResult shortestAccessibleRoute(const Graph&,int source,int destination);
 static void printOutdoorRoute(const Graph&,const RouteResult&);
};
#endif
