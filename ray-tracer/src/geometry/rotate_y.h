#pragma once

#include "hittable.h"
#include "../utils/common.h"

namespace raytracer {

/**
 * Rotation instance around Y axis
 * Wraps a hittable object and rotates it around the Y axis
 */
class RotateY : public Hittable {
public:
    RotateY(shared_ptr<Hittable> object, double angle) : object(object) {
        auto radians = degreesToRadians(angle);
        sinTheta = std::sin(radians);
        cosTheta = std::cos(radians);
        hasBox = object->boundingBox(0, 1, bbox);

        Point3 min(INFINITY_VAL, INFINITY_VAL, INFINITY_VAL);
        Point3 max(-INFINITY_VAL, -INFINITY_VAL, -INFINITY_VAL);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                    auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                    auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

                    auto newx = cosTheta * x + sinTheta * z;
                    auto newz = -sinTheta * x + cosTheta * z;

                    Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        // Change the ray from world space to object space
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cosTheta * r.origin()[0] - sinTheta * r.origin()[2];
        origin[2] = sinTheta * r.origin()[0] + cosTheta * r.origin()[2];

        direction[0] = cosTheta * r.direction()[0] - sinTheta * r.direction()[2];
        direction[2] = sinTheta * r.direction()[0] + cosTheta * r.direction()[2];

        Ray rotatedR(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where)
        if (!object->hit(rotatedR, tMin, tMax, rec))
            return false;

        // Change the intersection point from object space to world space
        auto p = rec.p;
        p[0] = cosTheta * rec.p[0] + sinTheta * rec.p[2];
        p[2] = -sinTheta * rec.p[0] + cosTheta * rec.p[2];

        // Change the normal from object space to world space
        auto normal = rec.normal;
        normal[0] = cosTheta * rec.normal[0] + sinTheta * rec.normal[2];
        normal[2] = -sinTheta * rec.normal[0] + cosTheta * rec.normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    bool boundingBox(double time0, double time1, AABB& outputBox) const override {
        outputBox = bbox;
        return hasBox;
    }

private:
    shared_ptr<Hittable> object;
    double sinTheta;
    double cosTheta;
    bool hasBox;
    AABB bbox;
};

} // namespace raytracer
