
#ifndef BUILDINGCONNECTOR_H
#define BUILDINGCONNECTOR_H

#include <string>
using namespace std;

class BuildingConnector {
public:
    int outdoorId;
    int indoorId;
    string building;

    BuildingConnector(int out, int in, string b) {
        outdoorId = out;
        indoorId = in;
        building = b;
    }
};

#endif

