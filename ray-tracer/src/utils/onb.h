#pragma once

#include "../core/vec3.h"

namespace raytracer {

/**
 * Orthonormal Basis
 * Constructs a local coordinate system from a given normal vector
 */
class ONB {
public:
    ONB() {}

    /**
     * Build an ONB from a given normal vector w
     */
    void buildFromW(const Vec3& w) {
        axis[2] = unitVector(w);
        Vec3 a = (fabs(axis[2].x()) > 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
        axis[1] = unitVector(cross(axis[2], a));
        axis[0] = cross(axis[2], axis[1]);
    }

    Vec3 u() const { return axis[0]; }
    Vec3 v() const { return axis[1]; }
    Vec3 w() const { return axis[2]; }

    /**
     * Transform a vector from local coordinates to world coordinates
     */
    Vec3 local(double a, double b, double c) const {
        return a * u() + b * v() + c * w();
    }

    Vec3 local(const Vec3& a) const {
        return a.x() * u() + a.y() * v() + a.z() * w();
    }

    Vec3 operator[](int i) const { return axis[i]; }

private:
    Vec3 axis[3];
};

} // namespace raytracer
