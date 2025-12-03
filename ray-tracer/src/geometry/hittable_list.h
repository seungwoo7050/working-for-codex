#pragma once

#include "hittable.h"
#include <vector>

namespace raytracer {

class HittableList : public Hittable {
public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object) {
        objects.push_back(object);
    }

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        HitRecord tempRec;
        bool hitAnything = false;
        auto closestSoFar = tMax;

        for (const auto& object : objects) {
            if (object->hit(r, tMin, closestSoFar, tempRec)) {
                hitAnything = true;
                closestSoFar = tempRec.t;
                rec = tempRec;
            }
        }

        return hitAnything;
    }

    bool boundingBox(double time0, double time1, AABB& outputBox) const override {
        if (objects.empty())
            return false;

        AABB tempBox;
        bool firstBox = true;

        for (const auto& object : objects) {
            if (!object->boundingBox(time0, time1, tempBox))
                return false;

            outputBox = firstBox ? tempBox : AABB::surroundingBox(outputBox, tempBox);
            firstBox = false;
        }

        return true;
    }

    double pdfValue(const Point3& origin, const Vec3& direction) const override {
        auto weight = 1.0 / objects.size();
        auto sum = 0.0;

        for (const auto& object : objects)
            sum += weight * object->pdfValue(origin, direction);

        return sum;
    }

    Vec3 random(const Point3& origin) const override {
        auto intSize = static_cast<int>(objects.size());
        return objects[randomInt(0, intSize - 1)]->random(origin);
    }
};

} // namespace raytracer
