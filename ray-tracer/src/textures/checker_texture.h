#pragma once

#include "texture.h"
#include "solid_color.h"
#include "../utils/common.h"
#include <cmath>

namespace raytracer {

/**
 * Checker texture
 * Alternates between two textures in a 3D checkerboard pattern
 */
class CheckerTexture : public Texture {
public:
    CheckerTexture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
        : invScale(1.0 / scale), even(even), odd(odd) {}

    CheckerTexture(double scale, const Color& c1, const Color& c2)
        : invScale(1.0 / scale),
          even(make_shared<SolidColor>(c1)),
          odd(make_shared<SolidColor>(c2)) {}

    Color value(double u, double v, const Point3& p) const override {
        auto xInteger = static_cast<int>(std::floor(invScale * p.x()));
        auto yInteger = static_cast<int>(std::floor(invScale * p.y()));
        auto zInteger = static_cast<int>(std::floor(invScale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double invScale;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};

} // namespace raytracer
