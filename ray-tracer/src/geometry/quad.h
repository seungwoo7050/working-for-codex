#pragma once

#include "hittable.h"
#include "../core/vec3.h"
#include "../utils/onb.h"

namespace raytracer {

/**
 * Quadrilateral (parallelogram) primitive
 * Defined by a corner point Q and two edge vectors u and v
 */
class Quad : public Hittable {
public:
    Quad(const Point3& Q, const Vec3& u, const Vec3& v, shared_ptr<Material> mat)
        : Q(Q), u(u), v(v), mat(mat) {
        auto n = cross(u, v);
        normal = unitVector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        area = n.length();
    }

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        auto denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane
        if (std::fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval
        auto t = (D - dot(normal, r.origin())) / denom;
        if (t < tMin || t > tMax)
            return false;

        // Determine if the hit point lies within the planar shape using its plane coordinates
        auto intersection = r.at(t);
        Vec3 planarHitptVector = intersection - Q;
        auto alpha = dot(w, cross(planarHitptVector, v));
        auto beta = dot(w, cross(u, planarHitptVector));

        if (!isInterior(alpha, beta, rec))
            return false;

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.setFaceNormal(r, normal);

        return true;
    }

    bool boundingBox(double time0, double time1, AABB& outputBox) const override {
        // Compute the bounding box of all four vertices
        outputBox = AABB(Q, Q + u + v);
        outputBox = AABB::surroundingBox(outputBox, AABB(Q + u, Q + v));
        return true;
    }

    double pdfValue(const Point3& origin, const Vec3& direction) const override {
        HitRecord rec;
        if (!this->hit(Ray(origin, direction), 0.001, INFINITY_VAL, rec))
            return 0;

        auto distanceSquared = rec.t * rec.t * direction.lengthSquared();
        auto cosine = std::fabs(dot(direction, rec.normal) / direction.length());

        return distanceSquared / (cosine * area);
    }

    Vec3 random(const Point3& origin) const override {
        auto p = Q + (randomDouble() * u) + (randomDouble() * v);
        return p - origin;
    }

    virtual bool isInterior(double a, double b, HitRecord& rec) const {
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true

        if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    Point3 Q;
    Vec3 u, v;
    Vec3 w;
    shared_ptr<Material> mat;
    Vec3 normal;
    double D;
    double area;
};

} // namespace raytracer
