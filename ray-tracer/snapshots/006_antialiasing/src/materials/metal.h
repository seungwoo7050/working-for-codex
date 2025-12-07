#pragma once

#include "material.h"
#include "../geometry/hittable.h"

namespace raytracer {

class Metal : public Material {
public:
    Metal(const Color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered)
    const override {
        Vec3 reflected = reflect(rIn.direction(), rec.normal);
        reflected = unitVector(reflected) + (fuzz * randomUnitVector());
        scattered = Ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    Color albedo;
    double fuzz;
};

} // namespace raytracer
