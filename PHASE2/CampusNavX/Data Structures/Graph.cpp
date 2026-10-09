#include "Graph.h"
#include <iostream>
#include <algorithm>
Graph::Graph(int size) { resize(size); }
void Graph::resize(int size) {
    if (size < 0) size=0;
    adjacency.resize(static_cast<std::size_t>(size+1));
    locations.resize(static_cast<std::size_t>(size+1));
}
void Graph::addLocation(const Location& location) {
    if(location.id<1) return;
    if(location.id>=static_cast<int>(locations.size())) resize(location.id);
    locations[location.id]=location;
}
void Graph::addEdge(int s,int d,double dist,double time,int blocked,int stairs,int elevator,int ramp) {
    if(s<1||d<1) return;
    if(s>=static_cast<int>(adjacency.size())||d>=static_cast<int>(adjacency.size())) resize(std::max(s,d));
    auto put=[](std::vector<Edge>& list,const Edge& e){
        auto it=std::find_if(list.begin(),list.end(),[&](const Edge& x){return x.destination==e.destination;});
        if(it==list.end()) list.push_back(e); else *it=e;
    };
    put(adjacency[s],Edge(d,dist,time,blocked,stairs,elevator,ramp));
    put(adjacency[d],Edge(s,dist,time,blocked,stairs,elevator,ramp));
}
const std::vector<Edge>& Graph::getNeighbours(int v) const {
    static const std::vector<Edge> empty;
    return (v>0 && v<static_cast<int>(adjacency.size())) ? adjacency[v] : empty;
}
int Graph::getSize() const { return static_cast<int>(adjacency.size())-1; }
const Location* Graph::getLocation(int id) const {
    return id>0 && id<static_cast<int>(locations.size()) && locations[id].id ? &locations[id] : nullptr;
}
bool Graph::blockPath(int s,int d){
    if(s<1||d<1||s>=static_cast<int>(adjacency.size())||d>=static_cast<int>(adjacency.size())) return false;
    bool found=false;
    for(int u:{s,d}) for(auto& e:adjacency[u]) if(e.destination==(u==s?d:s)){e.blocked=1;found=true;}
    return found;
}
bool Graph::unblockPath(int s,int d){
    if(s<1||d<1||s>=static_cast<int>(adjacency.size())||d>=static_cast<int>(adjacency.size())) return false;
    bool found=false;
    for(int u:{s,d}) for(auto& e:adjacency[u]) if(e.destination==(u==s?d:s)){e.blocked=0;found=true;}
    return found;
}
bool Graph::updatePath(int s,int d,double dist,double time){ bool found=false; for(int u:{s,d}) for(auto& e:adjacency[u]) if(e.destination==(u==s?d:s)){e.distance=dist;e.walkingTime=time;found=true;} return found; }
void Graph::display() const { for(int i=1;i<=getSize();++i){ auto p=getLocation(i); if(!p) continue; std::cout<<i<<". "<<p->name<<" ("<<p->type<<")\n"; } }
