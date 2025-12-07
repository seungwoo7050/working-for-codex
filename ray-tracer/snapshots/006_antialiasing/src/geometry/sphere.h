#pragma once

#include "hittable.h"
#include "../core/vec3.h"

namespace raytracer {

class Sphere : public Hittable {
public:
    Sphere(const Point3& center, double radius, shared_ptr<Material> mat)
        : center(center), radius(std::fmax(0, radius)), mat(mat) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        Vec3 oc = center - r.origin();
        auto a = r.direction().lengthSquared();
        auto h = dot(r.direction(), oc);
        auto c = oc.lengthSquared() - radius * radius;
        auto discriminant = h * h - a * c;

        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range
        auto root = (h - sqrtd) / a;
        if (root <= tMin || tMax <= root) {
            root = (h + sqrtd) / a;
            if (root <= tMin || tMax <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vec3 outwardNormal = (rec.p - center) / radius;
        rec.setFaceNormal(r, outwardNormal);
        rec.mat = mat;

        return true;
    }

private:
    Point3 center;
    double radius;
    shared_ptr<Material> mat;
};

} // namespace raytracer
