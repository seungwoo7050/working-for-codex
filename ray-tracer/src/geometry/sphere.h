#pragma once

#include "hittable.h"
#include "../core/vec3.h"
#include "../utils/onb.h"

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
        getSphereUV(outwardNormal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

    bool boundingBox(double time0, double time1, AABB& outputBox) const override {
        outputBox = AABB(
            center - Vec3(radius, radius, radius),
            center + Vec3(radius, radius, radius)
        );
        return true;
    }

    double pdfValue(const Point3& origin, const Vec3& direction) const override {
        HitRecord rec;
        if (!this->hit(Ray(origin, direction), 0.001, INFINITY_VAL, rec))
            return 0;

        auto cosThetaMax = std::sqrt(1 - radius * radius / (center - origin).lengthSquared());
        auto solidAngle = 2 * PI * (1 - cosThetaMax);

        return 1 / solidAngle;
    }

    Vec3 random(const Point3& origin) const override {
        Vec3 direction = center - origin;
        auto distanceSquared = direction.lengthSquared();
        ONB uvw;
        uvw.buildFromW(direction);
        return uvw.local(randomToSphere(radius, distanceSquared));
    }

private:
    Point3 center;
    double radius;
    shared_ptr<Material> mat;

    /**
     * Compute UV coordinates for a sphere
     * p: point on the sphere of radius 1, centered at the origin
     * u: returned value [0,1] of angle around the Y axis from X=-1
     * v: returned value [0,1] of angle from Y=-1 to Y=+1
     */
    static void getSphereUV(const Point3& p, double& u, double& v) {
        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + PI;

        u = phi / (2 * PI);
        v = theta / PI;
    }
};

} // namespace raytracer
