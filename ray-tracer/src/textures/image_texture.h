#pragma once

#include "texture.h"
#include "../utils/common.h"

namespace raytracer {

/**
 * Image texture
 * Maps an image onto objects (simplified version, no actual image loading)
 */
class ImageTexture : public Texture {
public:
    ImageTexture() : data(nullptr), width(0), height(0) {}

    // Constructor for simple procedural image (placeholder)
    ImageTexture(int width, int height)
        : width(width), height(height) {
        // For now, create a simple gradient
        bytesPerPixel = 3;
        data = new unsigned char[width * height * bytesPerPixel];
    }

    ~ImageTexture() {
        delete[] data;
    }

    Color value(double u, double v, const Point3& p) const override {
        // If we have no texture data, return solid cyan as a debugging aid
        if (data == nullptr)
            return Color(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [0,1]
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const auto colorScale = 1.0 / 255.0;
        auto pixel = data + j * bytesPerPixel * width + i * bytesPerPixel;

        return Color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }

private:
    unsigned char* data;
    int width, height;
    int bytesPerPixel;
};

} // namespace raytracer
