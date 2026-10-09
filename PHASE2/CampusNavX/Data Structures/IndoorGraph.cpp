#include "IndoorGraph.h"
#include <iostream>
#include <algorithm>
IndoorGraph::IndoorGraph(int size){resize(size);}
void IndoorGraph::resize(int size){if(size<0)size=0;adjacency.resize(static_cast<std::size_t>(size+1));locations.resize(static_cast<std::size_t>(size+1));}
void IndoorGraph::addLocation(const IndoorLocation& l){if(l.id<1)return;if(l.id>=static_cast<int>(locations.size()))resize(l.id);locations[l.id]=l;}
void IndoorGraph::addEdge(int s,int d,double dist,double time,int stairs,int elevator,int ramp,int blocked){
 if(s<1||d<1)return;if(s>=static_cast<int>(adjacency.size())||d>=static_cast<int>(adjacency.size()))resize(std::max(s,d));
 auto put=[](std::vector<IndoorEdge>& v,const IndoorEdge& e){auto it=std::find_if(v.begin(),v.end(),[&](const IndoorEdge& x){return x.destination==e.destination;});if(it==v.end())v.push_back(e);else *it=e;};
 put(adjacency[s],IndoorEdge(d,dist,time,stairs,elevator,ramp,blocked));put(adjacency[d],IndoorEdge(s,dist,time,stairs,elevator,ramp,blocked));
}
const std::vector<IndoorEdge>& IndoorGraph::getNeighbours(int v)const{static const std::vector<IndoorEdge> empty;return v>0&&v<static_cast<int>(adjacency.size())?adjacency[v]:empty;}
int IndoorGraph::getSize()const{return static_cast<int>(adjacency.size())-1;}
const IndoorLocation* IndoorGraph::getLocation(int id)const{return id>0&&id<static_cast<int>(locations.size())&&locations[id].id?&locations[id]:nullptr;}
bool IndoorGraph::blockPath(int s,int d){
 if(s<1||d<1||s>=static_cast<int>(adjacency.size())||d>=static_cast<int>(adjacency.size()))return false;
 bool f=false;for(int u:{s,d})for(auto& e:adjacency[u])if(e.destination==(u==s?d:s)){e.blocked=1;f=true;}return f;
}
bool IndoorGraph::unblockPath(int s,int d){
 if(s<1||d<1||s>=static_cast<int>(adjacency.size())||d>=static_cast<int>(adjacency.size()))return false;
 bool f=false;for(int u:{s,d})for(auto& e:adjacency[u])if(e.destination==(u==s?d:s)){e.blocked=0;f=true;}return f;
}
void IndoorGraph::display()const{for(int i=1;i<=getSize();++i){auto p=getLocation(i);if(p)std::cout<<i<<". "<<p->buildingName<<" | Floor "<<p->floor<<" | "<<p->roomName<<"\n";}}
