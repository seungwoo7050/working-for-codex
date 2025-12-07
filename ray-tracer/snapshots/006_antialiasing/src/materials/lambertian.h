#pragma once

#include "material.h"
#include "../geometry/hittable.h"

namespace raytracer {

class Lambertian : public Material {
public:
    Lambertian(const Color& albedo) : albedo(albedo) {}

    bool scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered)
    const override {
        auto scatterDirection = rec.normal + randomUnitVector();

        // Catch degenerate scatter direction
        if (scatterDirection.nearZero())
            scatterDirection = rec.normal;

        scattered = Ray(rec.p, scatterDirection);
        attenuation = albedo;
        return true;
    }

private:
    Color albedo;
};

} // namespace raytracer
