#pragma once

#include "../core/ray.h"
#include "../utils/common.h"
#include "aabb.h"

namespace raytracer {

class Material;

struct HitRecord {
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> mat;
    double t;
    double u;
    double v;
    bool frontFace;

    void setFaceNormal(const Ray& r, const Vec3& outwardNormal) {
        // Sets the hit record normal vector
        // NOTE: the parameter `outwardNormal` is assumed to have unit length
        frontFace = dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;

    /**
     * Compute the bounding box for this hittable object
     * @param time0 Start time for motion blur
     * @param time1 End time for motion blur
     * @param outputBox The computed bounding box
     * @return true if the object has a bounding box
     */
    virtual bool boundingBox(double time0, double time1, AABB& outputBox) const = 0;

    /**
     * PDF value for importance sampling
     * @param origin The origin point
     * @param direction The direction to evaluate
     * @return The PDF value
     */
    virtual double pdfValue(const Point3& origin, const Vec3& direction) const {
        return 0.0;
    }

    /**
     * Generate a random direction toward this object for importance sampling
     * @param origin The origin point
     * @return A random direction
     */
    virtual Vec3 random(const Point3& origin) const {
        return Vec3(1, 0, 0);
    }
};

} // namespace raytracer
