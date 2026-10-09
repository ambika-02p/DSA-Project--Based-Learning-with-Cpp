#include "EmergencyNavigation.h"
#include <string>
#include <algorithm>
#include <cctype>
RouteResult EmergencyNavigation::nearestAccessibleExit(const Graph& g,int source){RouteResult best;double bestDistance=1e100;for(int id=1;id<=g.getSize();++id){const Location* l=g.getLocation(id);if(!l)continue;std::string type=l->type;std::transform(type.begin(),type.end(),type.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});if(type.find("exit")==std::string::npos)continue;RouteResult r=Dijkstra::shortestPath(g,source,id,true);if(r.found&&r.distance<bestDistance){bestDistance=r.distance;best=r;}}return best;}
