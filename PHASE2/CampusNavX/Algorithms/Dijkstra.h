#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include <vector>
#include "../Data Structures/Graph.h"
#include "../Data Structures/IndoorGraph.h"
struct RouteResult { std::vector<int> path; double distance=0; double walkingTime=0; bool found=false; };
class Dijkstra {
public:
 static RouteResult shortestPath(const Graph&,int source,int destination,bool accessibleOnly=true);
 static RouteResult shortestIndoorPath(const IndoorGraph&,int source,int destination,bool accessibleOnly=true);
};
#endif
