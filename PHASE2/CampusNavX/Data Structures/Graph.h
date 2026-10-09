#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include <string>
#include "Edge.h"
#include "../models/Location.h"
class Graph {
    std::vector<std::vector<Edge>> adjacency;
    std::vector<Location> locations;
public:
    explicit Graph(int size=0);
    void resize(int size);
    void addLocation(const Location& location);
    void addEdge(int source,int destination,double distance,double walkingTime,
                 int blocked=0,int stairs=0,int elevator=0,int ramp=0);
    const std::vector<Edge>& getNeighbours(int vertex) const;
    int getSize() const;
    const Location* getLocation(int id) const;
    bool blockPath(int source,int destination);
    bool unblockPath(int source,int destination);
    bool updatePath(int source,int destination,double distance,double walkingTime);
    void display() const;
};
#endif
