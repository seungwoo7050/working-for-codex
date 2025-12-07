#pragma once

#include "../core/ray.h"
#include "../core/vec3.h"
#include "../core/color.h"

namespace raytracer {

struct HitRecord;

class Material {
public:
    virtual ~Material() = default;

    virtual bool scatter(
        const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered
    ) const {
        return false;
    }
};

} // namespace raytracer
