#pragma once

#include "material.h"
#include "../textures/texture.h"
#include "../textures/solid_color.h"

namespace raytracer {

/**
 * Diffuse light material
 * Emits light but does not scatter rays
 */
class DiffuseLight : public Material {
public:
    DiffuseLight(shared_ptr<Texture> emit) : emit(emit) {}
    DiffuseLight(const Color& emit) : emit(make_shared<SolidColor>(emit)) {}

    bool scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered)
    const override {
        return false;
    }

    Color emitted(double u, double v, const Point3& p) const override {
        return emit->value(u, v, p);
    }

private:
    shared_ptr<Texture> emit;
};

} // namespace raytracer
