#ifndef DFS_H
#define DFS_H
#include <vector>
#include "../Data Structures/Graph.h"
#include "../Data Structures/IndoorGraph.h"
class DFS {
public:
 static std::vector<int> findPath(const Graph&,int source,int destination,bool accessibleOnly=true);
 static std::vector<int> findIndoorPath(const IndoorGraph&,int source,int destination,bool accessibleOnly=true);
};
#endif
