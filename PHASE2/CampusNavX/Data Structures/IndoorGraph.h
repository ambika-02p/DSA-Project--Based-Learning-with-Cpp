#ifndef INDOOR_GRAPH_H
#define INDOOR_GRAPH_H
#include <vector>
#include "IndoorEdge.h"
#include "../models/IndoorLocation.h"
class IndoorGraph {
    std::vector<std::vector<IndoorEdge>> adjacency;
    std::vector<IndoorLocation> locations;
public:
    explicit IndoorGraph(int size=0);
    void resize(int size);
    void addLocation(const IndoorLocation& location);
    void addEdge(int source,int destination,double distance,double walkingTime,
                 int stairs=0,int elevator=0,int ramp=0,int blocked=0);
    const std::vector<IndoorEdge>& getNeighbours(int vertex) const;
    int getSize() const;
    const IndoorLocation* getLocation(int id) const;
    bool blockPath(int source,int destination);
    bool unblockPath(int source,int destination);
    void display() const;
};
#endif
