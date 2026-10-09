#include "AccessibleNavigation.h"
RouteResult AccessibleNavigation::findRoute(const Graph& g,int s,int d){return Dijkstra::shortestPath(g,s,d,true);}
RouteResult AccessibleNavigation::findIndoorRoute(const IndoorGraph& g,int s,int d){return Dijkstra::shortestIndoorPath(g,s,d,true);}
