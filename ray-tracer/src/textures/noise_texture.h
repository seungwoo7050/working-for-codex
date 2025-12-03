#pragma once

#include "texture.h"
#include "perlin.h"
#include <cmath>

namespace raytracer {

/**
 * Noise texture using Perlin noise
 * Creates a natural-looking turbulent pattern
 */
class NoiseTexture : public Texture {
public:
    NoiseTexture() {}
    NoiseTexture(double scale) : scale(scale) {}

    Color value(double u, double v, const Point3& p) const override {
        auto s = scale * p;
        return Color(1, 1, 1) * 0.5 * (1 + std::sin(s.z() + 10 * noise.turb(s)));
    }

private:
    Perlin noise;
    double scale;
};

} // namespace raytracer
