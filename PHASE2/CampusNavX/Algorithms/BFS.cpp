#include "BFS.h"
#include <queue>
#include <algorithm>
template<class G> static std::vector<int> bfsImpl(const G& g,int s,int d,bool accessible){int n=g.getSize();if(s<1||d<1||s>n||d>n)return {};std::vector<int> parent(n+1,-1);std::vector<char> seen(n+1,0);std::queue<int> q;q.push(s);seen[s]=1;while(!q.empty()){int u=q.front();q.pop();if(u==d)break;for(const auto& e:g.getNeighbours(u)){if(e.destination<1||e.destination>n||seen[e.destination]||e.blocked)continue;if(accessible&&e.stairs&&!e.elevator&&!e.ramp)continue;seen[e.destination]=1;parent[e.destination]=u;q.push(e.destination);}}if(!seen[d])return {};std::vector<int> path;for(int v=d;v!=-1;v=parent[v])path.push_back(v);std::reverse(path.begin(),path.end());return path;}
std::vector<int> BFS::findPath(const Graph& g,int s,int d,bool a){return bfsImpl(g,s,d,a);}
std::vector<int> BFS::findIndoorPath(const IndoorGraph& g,int s,int d,bool a){return bfsImpl(g,s,d,a);}
