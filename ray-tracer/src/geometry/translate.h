#pragma once

#include "hittable.h"

namespace raytracer {

/**
 * Translation instance
 * Wraps a hittable object and translates it by an offset
 */
class Translate : public Hittable {
public:
    Translate(shared_ptr<Hittable> object, const Vec3& offset)
        : object(object), offset(offset) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        // Move the ray backwards by the offset
        Ray movedR(r.origin() - offset, r.direction(), r.time());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!object->hit(movedR, tMin, tMax, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p = rec.p + offset;

        return true;
    }

    bool boundingBox(double time0, double time1, AABB& outputBox) const override {
        if (!object->boundingBox(time0, time1, outputBox))
            return false;

        outputBox = AABB(
            outputBox.min() + offset,
            outputBox.max() + offset
        );

        return true;
    }

private:
    shared_ptr<Hittable> object;
    Vec3 offset;
};

} // namespace raytracer
