#include "DFS.h"
#include <algorithm>
template<class G> static std::vector<int> dfsImpl(const G& g,int s,int d,bool accessible){int n=g.getSize();if(s<1||d<1||s>n||d>n)return {};std::vector<int> parent(n+1,-1);std::vector<char> seen(n+1,0);std::vector<int> st{ s };seen[s]=1;while(!st.empty()){int u=st.back();st.pop_back();if(u==d)break;const auto& es=g.getNeighbours(u);for(auto it=es.rbegin();it!=es.rend();++it){const auto& e=*it;if(e.destination<1||e.destination>n||seen[e.destination]||e.blocked||(accessible&&e.stairs&&!e.elevator&&!e.ramp))continue;seen[e.destination]=1;parent[e.destination]=u;st.push_back(e.destination);}}if(!seen[d])return {};std::vector<int> p;for(int v=d;v!=-1;v=parent[v])p.push_back(v);std::reverse(p.begin(),p.end());return p;}
std::vector<int> DFS::findPath(const Graph& g,int s,int d,bool a){return dfsImpl(g,s,d,a);}
std::vector<int> DFS::findIndoorPath(const IndoorGraph& g,int s,int d,bool a){return dfsImpl(g,s,d,a);}
