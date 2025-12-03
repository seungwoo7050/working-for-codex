#pragma once

#include "material.h"
#include "../geometry/hittable.h"
#include "../textures/texture.h"
#include "../textures/solid_color.h"
#include "../utils/pdf.h"
#include "../utils/onb.h"

namespace raytracer {

class Lambertian : public Material {
public:
    Lambertian(const Color& albedo) : albedo(make_shared<SolidColor>(albedo)) {}
    Lambertian(shared_ptr<Texture> albedo) : albedo(albedo) {}

    bool scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered)
    const override {
        auto scatterDirection = rec.normal + randomUnitVector();

        // Catch degenerate scatter direction
        if (scatterDirection.nearZero())
            scatterDirection = rec.normal;

        scattered = Ray(rec.p, scatterDirection);
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    bool scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override {
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdfPtr = make_shared<CosinePDF>(rec.normal);
        srec.skipPdf = false;
        return true;
    }

    double scatteringPdf(const Ray& rIn, const HitRecord& rec, const Ray& scattered)
    const override {
        auto cosine = dot(rec.normal, unitVector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / PI;
    }

private:
    shared_ptr<Texture> albedo;
};

} // namespace raytracer
