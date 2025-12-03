#pragma once

#include "../core/vec3.h"
#include "../core/ray.h"
#include "onb.h"
#include "common.h"
#include "../geometry/hittable.h"
#include "../geometry/hittable_list.h"
#include <memory>

namespace raytracer {

/**
 * Probability Density Function base class
 */
class PDF {
public:
    virtual ~PDF() = default;

    /**
     * Returns the PDF value for a given direction
     */
    virtual double value(const Vec3& direction) const = 0;

    /**
     * Generates a random direction according to the PDF
     */
    virtual Vec3 generate() const = 0;
};

/**
 * Cosine-weighted PDF for Lambertian scattering
 * PDF proportional to cos(theta) where theta is angle from normal
 */
class CosinePDF : public PDF {
public:
    CosinePDF(const Vec3& w) {
        uvw.buildFromW(w);
    }

    double value(const Vec3& direction) const override {
        auto cosineTheta = dot(unitVector(direction), uvw.w());
        return fmax(0, cosineTheta / PI);
    }

    Vec3 generate() const override {
        return uvw.local(randomCosineDirection());
    }

private:
    ONB uvw;
};

/**
 * Uniform sphere PDF
 */
class SpherePDF : public PDF {
public:
    SpherePDF() {}

    double value(const Vec3& direction) const override {
        return 1.0 / (4.0 * PI);
    }

    Vec3 generate() const override {
        return randomUnitVector();
    }
};

/**
 * PDF that samples directions toward a hittable object (importance sampling)
 */
class HittablePDF : public PDF {
public:
    HittablePDF(const Hittable& objects, const Point3& origin)
        : objects(objects), origin(origin) {}

    double value(const Vec3& direction) const override {
        return objects.pdfValue(origin, direction);
    }

    Vec3 generate() const override {
        return objects.random(origin);
    }

private:
    const Hittable& objects;
    Point3 origin;
};

/**
 * Mixture PDF that combines two PDFs
 * Samples from each PDF with 50% probability
 */
class MixturePDF : public PDF {
public:
    MixturePDF(shared_ptr<PDF> p0, shared_ptr<PDF> p1)
        : p{p0, p1} {}

    double value(const Vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    Vec3 generate() const override {
        if (randomDouble() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }

private:
    shared_ptr<PDF> p[2];
};

} // namespace raytracer
