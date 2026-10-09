#ifndef CAMPUS_MANAGER_H
#define CAMPUS_MANAGER_H
#include "../Data Structures/Graph.h"
#include "../Data Structures/IndoorGraph.h"
#include "../algorithms/Dijkstra.h"
class CampusManager {
    Graph& outdoor;
    IndoorGraph& indoor;
public:
    CampusManager(Graph& g,IndoorGraph& i):outdoor(g),indoor(i){}
    RouteResult route(int source,int destination) const { return Dijkstra::shortestPath(outdoor,source,destination,true); }
    RouteResult indoorRoute(int source,int destination) const { return Dijkstra::shortestIndoorPath(indoor,source,destination,true); }
    bool blockOutdoorPath(int source,int destination){return outdoor.blockPath(source,destination);}
    bool unblockOutdoorPath(int source,int destination){return outdoor.unblockPath(source,destination);}
};
#endif
