
#ifndef INDOORLOCATION_H
#define INDOORLOCATION_H

#include <string>
using namespace std;

class IndoorLocation {
public:
    int id;
    string name;
    string building;
    int floor;

    IndoorLocation() {
        id = 0;
        name = "";
        building = "";
        floor = 0;
    }

    IndoorLocation(int i, string n, string b, int f) {
        id = i;
        name = n;
        building = b;
        floor = f;
    }
};

#endif
