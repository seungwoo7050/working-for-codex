#pragma once

#include "hittable.h"
#include "hittable_list.h"
#include "aabb.h"
#include "../utils/common.h"
#include <algorithm>

namespace raytracer {

/**
 * BVH (Bounding Volume Hierarchy) Node
 * Accelerates ray-object intersection tests by organizing objects in a tree structure
 */
class BVHNode : public Hittable {
public:
    BVHNode() {}

    BVHNode(HittableList& list, double time0, double time1)
        : BVHNode(list.objects, 0, list.objects.size(), time0, time1) {}

    BVHNode(
        std::vector<shared_ptr<Hittable>>& srcObjects,
        size_t start, size_t end, double time0, double time1
    ) {
        auto objects = srcObjects; // Create a modifiable array of the source scene objects

        int axis = randomInt(0, 2);
        auto comparator = (axis == 0) ? boxXCompare
                        : (axis == 1) ? boxYCompare
                                      : boxZCompare;

        size_t objectSpan = end - start;

        if (objectSpan == 1) {
            left = right = objects[start];
        } else if (objectSpan == 2) {
            if (comparator(objects[start], objects[start + 1])) {
                left = objects[start];
                right = objects[start + 1];
            } else {
                left = objects[start + 1];
                right = objects[start];
            }
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + objectSpan / 2;
            left = make_shared<BVHNode>(objects, start, mid, time0, time1);
            right = make_shared<BVHNode>(objects, mid, end, time0, time1);
        }

        AABB boxLeft, boxRight;

        if (!left->boundingBox(time0, time1, boxLeft)
         || !right->boundingBox(time0, time1, boxRight)) {
            std::cerr << "No bounding box in BVHNode constructor.\n";
        }

        box = AABB::surroundingBox(boxLeft, boxRight);
    }

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        if (!box.hit(r, tMin, tMax))
            return false;

        bool hitLeft = left->hit(r, tMin, tMax, rec);
        bool hitRight = right->hit(r, tMin, hitLeft ? rec.t : tMax, rec);

        return hitLeft || hitRight;
    }

    bool boundingBox(double time0, double time1, AABB& outputBox) const override {
        outputBox = box;
        return true;
    }

private:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB box;

    static bool boxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis) {
        AABB boxA;
        AABB boxB;

        if (!a->boundingBox(0, 0, boxA) || !b->boundingBox(0, 0, boxB))
            std::cerr << "No bounding box in BVHNode constructor.\n";

        return boxA.min()[axis] < boxB.min()[axis];
    }

    static bool boxXCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return boxCompare(a, b, 0);
    }

    static bool boxYCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return boxCompare(a, b, 1);
    }

    static bool boxZCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
        return boxCompare(a, b, 2);
    }
};

} // namespace raytracer
