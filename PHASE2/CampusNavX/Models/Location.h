#ifndef LOCATION_H
#define LOCATION_H
#include <string>
struct Location {
    int id = 0;
    std::string name;
    std::string type;
    int floor = 0;
    Location() = default;
    Location(int i, const std::string& n, const std::string& t, int f=0)
        : id(i), name(n), type(t), floor(f) {}
};
#endif
