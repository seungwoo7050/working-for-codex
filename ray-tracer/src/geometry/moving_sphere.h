#pragma once

#include "hittable.h"
#include "../core/vec3.h"

namespace raytracer {

/**
 * Moving sphere for motion blur
 * Sphere that moves linearly from center0 to center1 during time0 to time1
 */
class MovingSphere : public Hittable {
public:
    MovingSphere(
        const Point3& center0, const Point3& center1,
        double time0, double time1,
        double radius, shared_ptr<Material> mat)
        : center0(center0), center1(center1),
          time0(time0), time1(time1),
          radius(std::fmax(0, radius)), mat(mat) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        Vec3 oc = center(r.time()) - r.origin();
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
        Vec3 outwardNormal = (rec.p - center(r.time())) / radius;
        rec.setFaceNormal(r, outwardNormal);
        getSphereUV(outwardNormal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

    bool boundingBox(double time0, double time1, AABB& outputBox) const override {
        AABB box0(
            center(time0) - Vec3(radius, radius, radius),
            center(time0) + Vec3(radius, radius, radius)
        );
        AABB box1(
            center(time1) - Vec3(radius, radius, radius),
            center(time1) + Vec3(radius, radius, radius)
        );
        outputBox = AABB::surroundingBox(box0, box1);
        return true;
    }

    Point3 center(double time) const {
        // Linearly interpolate from center0 to center1 according to time
        return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
    }

private:
    Point3 center0, center1;
    double time0, time1;
    double radius;
    shared_ptr<Material> mat;

    static void getSphereUV(const Point3& p, double& u, double& v) {
        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + PI;

        u = phi / (2 * PI);
        v = theta / PI;
    }
};

} // namespace raytracer
