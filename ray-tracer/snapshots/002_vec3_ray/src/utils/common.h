#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings
using std::make_shared;
using std::shared_ptr;

// Constants
const double PI = 3.1415926535897932385;
const double INFINITY_VAL = std::numeric_limits<double>::infinity();

// Utility Functions
inline double degreesToRadians(double degrees) {
    return degrees * PI / 180.0;
}

inline double randomDouble() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double randomDouble(double min, double max) {
    return min + (max - min) * randomDouble();
}

inline int randomInt(int min, int max) {
    return static_cast<int>(randomDouble(min, max + 1));
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
