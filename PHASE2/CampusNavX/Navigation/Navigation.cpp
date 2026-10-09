#include "Navigation.h"
#include <iostream>
RouteResult Navigation::shortestAccessibleRoute(const Graph& g,int s,int d){return Dijkstra::shortestPath(g,s,d,true);}
void Navigation::printOutdoorRoute(const Graph& g,const RouteResult& r){if(!r.found){std::cout<<"No accessible route found. Check blocked paths or stairs-only connections.\n";return;}std::cout<<"Accessible shortest route: ";for(std::size_t i=0;i<r.path.size();++i){if(i)std::cout<<" -> ";auto p=g.getLocation(r.path[i]);std::cout<<(p?p->name:std::to_string(r.path[i]));}std::cout<<"\nDistance: "<<r.distance<<" m\nEstimated walking time: "<<r.walkingTime<<" min\n";}
