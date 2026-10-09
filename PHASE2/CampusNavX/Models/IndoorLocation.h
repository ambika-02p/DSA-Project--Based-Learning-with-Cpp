#ifndef INDOOR_LOCATION_H
#define INDOOR_LOCATION_H
#include <string>
struct IndoorLocation {
    int id = 0;
    int buildingId = 0;
    std::string buildingName;
    int floor = 0;
    std::string roomName;
    std::string type;
    IndoorLocation() = default;
    IndoorLocation(int i, int b, const std::string& bn, int f,
                   const std::string& rn, const std::string& t)
        : id(i), buildingId(b), buildingName(bn), floor(f), roomName(rn), type(t) {}
};
#endif
