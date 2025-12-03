#pragma once

#include "material.h"
#include "../textures/texture.h"
#include "../textures/solid_color.h"
#include "../geometry/hittable.h"

namespace raytracer {

/**
 * Isotropic material
 * Scatters rays uniformly in all directions (used for volumes)
 */
class Isotropic : public Material {
public:
    Isotropic(const Color& albedo) : albedo(make_shared<SolidColor>(albedo)) {}
    Isotropic(shared_ptr<Texture> albedo) : albedo(albedo) {}

    bool scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered)
    const override {
        scattered = Ray(rec.p, randomUnitVector(), rIn.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

private:
    shared_ptr<Texture> albedo;
};

} // namespace raytracer
