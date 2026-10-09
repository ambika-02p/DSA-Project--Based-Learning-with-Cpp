#include "IndoorNavigation.h"
#include <iostream>
RouteResult IndoorNavigation::shortestAccessibleRoute(const IndoorGraph& g,int s,int d){return Dijkstra::shortestIndoorPath(g,s,d,true);}
void IndoorNavigation::printIndoorRoute(const IndoorGraph& g,const RouteResult& r){if(!r.found){std::cout<<"No accessible indoor route found.\n";return;}std::cout<<"Accessible indoor route: ";for(std::size_t i=0;i<r.path.size();++i){if(i)std::cout<<" -> ";auto p=g.getLocation(r.path[i]);if(p)std::cout<<p->roomName<<" ["<<p->buildingName<<", floor "<<p->floor<<"]";else std::cout<<r.path[i];}std::cout<<"\nDistance: "<<r.distance<<" m\nEstimated walking time: "<<r.walkingTime<<" min\n";}
