#pragma once

#include "../core/ray.h"
#include "../utils/common.h"

namespace raytracer {

class Material;

struct HitRecord {
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> mat;
    double t;
    bool frontFace;

    void setFaceNormal(const Ray& r, const Vec3& outwardNormal) {
        frontFace = dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
};

} // namespace raytracer
