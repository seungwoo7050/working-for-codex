#pragma once

#include "../core/vec3.h"
#include "../core/color.h"

namespace raytracer {

/**
 * Texture interface
 * Provides color values at different points in space
 */
class Texture {
public:
    virtual ~Texture() = default;

    /**
     * Get the color value at a specific point
     * @param u Texture coordinate u (0-1)
     * @param v Texture coordinate v (0-1)
     * @param p Point in 3D space
     * @return Color at the given location
     */
    virtual Color value(double u, double v, const Point3& p) const = 0;
};

} // namespace raytracer
