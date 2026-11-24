
#ifndef LOCATION_H
#define LOCATION_H

#include <string>
using namespace std;

class Location {
public:
    int id;
    string name;
    string type;

    // Default constructor
    Location() {
        id = 0;
        name = "";
        type = "";
    }

    // Parameterized constructor
    Location(int i, string n, string t) {
        id = i;
        name = n;
        type = t;
    }
};

#endif
