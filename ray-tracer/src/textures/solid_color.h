#pragma once

#include "texture.h"

namespace raytracer {

/**
 * Solid color texture
 * Returns the same color at all points
 */
class SolidColor : public Texture {
public:
    SolidColor() {}
    SolidColor(const Color& c) : colorValue(c) {}
    SolidColor(double red, double green, double blue)
        : SolidColor(Color(red, green, blue)) {}

    Color value(double u, double v, const Point3& p) const override {
        return colorValue;
    }

private:
    Color colorValue;
};

} // namespace raytracer
