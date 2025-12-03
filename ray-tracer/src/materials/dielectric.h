#pragma once

#include "material.h"
#include "../geometry/hittable.h"

namespace raytracer {

class Dielectric : public Material {
public:
    Dielectric(double refractionIndex) : refractionIndex(refractionIndex) {}

    bool scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered)
    const override {
        attenuation = Color(1.0, 1.0, 1.0);
        double ri = rec.frontFace ? (1.0 / refractionIndex) : refractionIndex;

        Vec3 unitDirection = unitVector(rIn.direction());
        double cosTheta = std::fmin(dot(-unitDirection, rec.normal), 1.0);
        double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = ri * sinTheta > 1.0;
        Vec3 direction;

        if (cannotRefract || reflectance(cosTheta, ri) > randomDouble())
            direction = reflect(unitDirection, rec.normal);
        else
            direction = refract(unitDirection, rec.normal, ri);

        scattered = Ray(rec.p, direction);
        return true;
    }

    bool scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const override {
        srec.attenuation = Color(1.0, 1.0, 1.0);
        srec.pdfPtr = nullptr;
        srec.skipPdf = true;
        double ri = rec.frontFace ? (1.0 / refractionIndex) : refractionIndex;

        Vec3 unitDirection = unitVector(rIn.direction());
        double cosTheta = std::fmin(dot(-unitDirection, rec.normal), 1.0);
        double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = ri * sinTheta > 1.0;
        Vec3 direction;

        if (cannotRefract || reflectance(cosTheta, ri) > randomDouble())
            direction = reflect(unitDirection, rec.normal);
        else
            direction = refract(unitDirection, rec.normal, ri);

        srec.skipPdfRay = Ray(rec.p, direction);
        return true;
    }

private:
    double refractionIndex;

    static double reflectance(double cosine, double refractionIndex) {
        // Use Schlick's approximation for reflectance
        auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

} // namespace raytracer
