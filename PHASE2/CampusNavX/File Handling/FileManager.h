#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include <string>
#include <vector>
#include "../Data Structures/Graph.h"
#include "../Data Structures/IndoorGraph.h"
#include "../models/IndoorLocation.h"
class FileManager {
public:
 static bool loadLocations(Graph&,const std::string& filename);
 static bool loadPathways(Graph&,const std::string& filename);
 static bool loadIndoorLocations(std::vector<IndoorLocation>&,const std::string& filename);
 static bool loadIndoorPathways(IndoorGraph&,const std::string& filename);
 static bool savePathways(const Graph&,const std::string& filename);
 static bool saveIndoorPathways(const IndoorGraph&,const std::string& filename);
};
#endif
