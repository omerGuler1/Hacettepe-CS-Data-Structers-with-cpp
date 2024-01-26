#include "Sector.h"

// Constructor implementation

#include <cmath>  // Include this for sqrt and pow functions

Sector::Sector(int x, int y, int z) : x(x), y(y), z(z), left(nullptr), right(nullptr), parent(nullptr), color(RED) {
    // Calculate the distance from Earth
    double distance = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    distance_from_earth = distance;

    // Generate the sector code
    sector_code = std::to_string(static_cast<int>(distance));  // Truncate the distance to an integer

    // Append coordinate components to the sector code
    sector_code += (x == 0) ? "S" : (x > 0) ? "R" : "L";
    sector_code += (y == 0) ? "S" : (y > 0) ? "U" : "D";
    sector_code += (z == 0) ? "S" : (z > 0) ? "F" : "B";
}


Sector::~Sector() {
    // TODO: Free any dynamically allocated memory if necessary
}

Sector& Sector::operator=(const Sector& other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        sector_code = other.sector_code;
        // Copy other members if they exist
    }
    return *this;
}


bool Sector::operator==(const Sector& other) const {
    return (x == other.x && y == other.y && z == other.z);
}

bool Sector::operator!=(const Sector& other) const {
    return !(*this == other);
}