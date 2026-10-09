#ifndef INDOOR_NAVIGATION_H
#define INDOOR_NAVIGATION_H
#include "../Data Structures/IndoorGraph.h"
#include "../algorithms/Dijkstra.h"
class IndoorNavigation {
public:
 static RouteResult shortestAccessibleRoute(const IndoorGraph&,int source,int destination);
 static void printIndoorRoute(const IndoorGraph&,const RouteResult&);
};
#endif
