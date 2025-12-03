#pragma once

#include "../core/ray.h"
#include "../core/vec3.h"
#include "../utils/common.h"

namespace raytracer {

/**
 * Axis-Aligned Bounding Box class
 * Used for accelerating ray-object intersection tests with BVH
 */
class AABB {
public:
    Point3 minimum;
    Point3 maximum;

    AABB() {}
    AABB(const Point3& a, const Point3& b) : minimum(a), maximum(b) {}

    Point3 min() const { return minimum; }
    Point3 max() const { return maximum; }

    /**
     * Check if a ray hits this bounding box
     * Uses Andrew Kensler's optimized algorithm
     */
    bool hit(const Ray& r, double tMin, double tMax) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0 / r.direction()[a];
            auto t0 = (minimum[a] - r.origin()[a]) * invD;
            auto t1 = (maximum[a] - r.origin()[a]) * invD;

            if (invD < 0.0)
                std::swap(t0, t1);

            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;

            if (tMax <= tMin)
                return false;
        }
        return true;
    }

    /**
     * Compute the bounding box that contains two boxes
     */
    static AABB surroundingBox(const AABB& box0, const AABB& box1) {
        Point3 small(
            std::fmin(box0.min().x(), box1.min().x()),
            std::fmin(box0.min().y(), box1.min().y()),
            std::fmin(box0.min().z(), box1.min().z())
        );

        Point3 big(
            std::fmax(box0.max().x(), box1.max().x()),
            std::fmax(box0.max().y(), box1.max().y()),
            std::fmax(box0.max().z(), box1.max().z())
        );

        return AABB(small, big);
    }
};

} // namespace raytracer
