#pragma once

#include "hittable.h"
#include "../materials/isotropic.h"
#include "../utils/common.h"
#include <cmath>

namespace raytracer {

/**
 * Constant medium (volume)
 * Represents fog, smoke, mist, etc. with constant density
 */
class ConstantMedium : public Hittable {
public:
    ConstantMedium(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> texture)
        : boundary(boundary),
          negInvDensity(-1 / density),
          phaseFunction(make_shared<Isotropic>(texture)) {}

    ConstantMedium(shared_ptr<Hittable> boundary, double density, const Color& albedo)
        : boundary(boundary),
          negInvDensity(-1 / density),
          phaseFunction(make_shared<Isotropic>(albedo)) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        // Print occasional samples when debugging. To enable, set enableDebug true.
        const bool enableDebug = false;
        const bool debugging = enableDebug && randomDouble() < 0.00001;

        HitRecord rec1, rec2;

        if (!boundary->hit(r, -INFINITY_VAL, INFINITY_VAL, rec1))
            return false;

        if (!boundary->hit(r, rec1.t + 0.0001, INFINITY_VAL, rec2))
            return false;

        if (debugging) std::cerr << "\ntMin=" << rec1.t << ", tMax=" << rec2.t << '\n';

        if (rec1.t < tMin) rec1.t = tMin;
        if (rec2.t > tMax) rec2.t = tMax;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto rayLength = r.direction().length();
        auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
        auto hitDistance = negInvDensity * std::log(randomDouble());

        if (hitDistance > distanceInsideBoundary)
            return false;

        rec.t = rec1.t + hitDistance / rayLength;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::cerr << "hitDistance = " << hitDistance << '\n'
                      << "rec.t = " << rec.t << '\n'
                      << "rec.p = " << rec.p << '\n';
        }

        rec.normal = Vec3(1, 0, 0);  // arbitrary
        rec.frontFace = true;         // also arbitrary
        rec.mat = phaseFunction;

        return true;
    }

    bool boundingBox(double time0, double time1, AABB& outputBox) const override {
        return boundary->boundingBox(time0, time1, outputBox);
    }

private:
    shared_ptr<Hittable> boundary;
    double negInvDensity;
    shared_ptr<Material> phaseFunction;
};

} // namespace raytracer
