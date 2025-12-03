#pragma once

#include "../core/ray.h"
#include "../core/vec3.h"
#include "../core/color.h"

namespace raytracer {

struct HitRecord;
struct ScatterRecord;

class Material {
public:
    virtual ~Material() = default;

    virtual bool scatter(
        const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered
    ) const {
        return false;
    }

    virtual bool scatter(const Ray& rIn, const HitRecord& rec, ScatterRecord& srec) const {
        return false;
    }

    virtual double scatteringPdf(
        const Ray& rIn, const HitRecord& rec, const Ray& scattered
    ) const {
        return 0;
    }

    virtual Color emitted(double u, double v, const Point3& p) const {
        return Color(0, 0, 0);
    }
};

struct ScatterRecord {
    Color attenuation;
    shared_ptr<class PDF> pdfPtr;
    bool skipPdf;
    Ray skipPdfRay;
};

} // namespace raytracer
