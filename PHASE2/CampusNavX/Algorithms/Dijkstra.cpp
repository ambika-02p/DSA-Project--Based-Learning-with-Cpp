#include "Dijkstra.h"
#include "MinHeap.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
template<class G> static RouteResult dijkstraImpl(const G& g,int s,int d,bool accessible){RouteResult out;int n=g.getSize();if(s<1||d<1||s>n||d>n)return out;const double INF=std::numeric_limits<double>::infinity();std::vector<double> dist(n+1,INF),time(n+1,INF);std::vector<int> prev(n+1,-1);MinHeap heap;dist[s]=0;time[s]=0;heap.push(s,0);while(!heap.empty()){HeapNode cur=heap.pop();int u=cur.vertex;if(cur.distance>dist[u])continue;if(u==d)break;for(const auto& e:g.getNeighbours(u)){int v=e.destination;if(v<1||v>n||e.blocked||(accessible&&e.stairs&&!e.elevator&&!e.ramp))continue;double nd=dist[u]+e.distance;if(nd<dist[v]){dist[v]=nd;time[v]=time[u]+e.walkingTime;prev[v]=u;heap.push(v,nd);}}}if(!std::isfinite(dist[d]))return out;for(int v=d;v!=-1;v=prev[v])out.path.push_back(v);std::reverse(out.path.begin(),out.path.end());out.distance=dist[d];out.walkingTime=time[d];out.found=true;return out;}
RouteResult Dijkstra::shortestPath(const Graph& g,int s,int d,bool a){return dijkstraImpl(g,s,d,a);}
RouteResult Dijkstra::shortestIndoorPath(const IndoorGraph& g,int s,int d,bool a){return dijkstraImpl(g,s,d,a);}
