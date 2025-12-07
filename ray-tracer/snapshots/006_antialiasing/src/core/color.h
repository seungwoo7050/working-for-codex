#pragma once

#include "vec3.h"
#include <iostream>

namespace raytracer {

using Color = Vec3;

inline double linearToGamma(double linearComponent) {
    if (linearComponent > 0)
        return std::sqrt(linearComponent);
    return 0;
}

inline void writeColor(std::ostream& out, const Color& pixelColor) {
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    // Apply a linear to gamma transform for gamma 2
    r = linearToGamma(r);
    g = linearToGamma(g);
    b = linearToGamma(b);

    // Translate the [0,1] component values to the byte range [0,255]
    static const double intensity = 255.999;
    int rbyte = static_cast<int>(intensity * clamp(r, 0.0, 0.999));
    int gbyte = static_cast<int>(intensity * clamp(g, 0.0, 0.999));
    int bbyte = static_cast<int>(intensity * clamp(b, 0.0, 0.999));

    // Write out the pixel color components
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

} // namespace raytracer
