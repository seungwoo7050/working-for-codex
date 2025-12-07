/**
 * Ray Tracer - v1.0.1-1.0.2
 * 
 * Vec3 and Ray class demonstration.
 * Produces a gradient based on ray direction.
 */
#include "core/vec3.h"
#include "core/ray.h"
#include <iostream>

using namespace raytracer;

Color rayColor(const Ray& r) {
    Vec3 unitDirection = unitVector(r.direction());
    auto a = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main() {
    // Image
    auto aspectRatio = 16.0 / 9.0;
    int imageWidth = 400;
    int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    imageHeight = (imageHeight < 1) ? 1 : imageHeight;

    // Camera
    auto focalLength = 1.0;
    auto viewportHeight = 2.0;
    auto viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / imageHeight);
    auto cameraCenter = Point3(0, 0, 0);

    // Viewport vectors
    auto viewportU = Vec3(viewportWidth, 0, 0);
    auto viewportV = Vec3(0, -viewportHeight, 0);

    auto pixelDeltaU = viewportU / imageWidth;
    auto pixelDeltaV = viewportV / imageHeight;

    auto viewportUpperLeft = cameraCenter - Vec3(0, 0, focalLength) - viewportU/2 - viewportV/2;
    auto pixel00Loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

    // Render
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    for (int j = 0; j < imageHeight; j++) {
        std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
        for (int i = 0; i < imageWidth; i++) {
            auto pixelCenter = pixel00Loc + (i * pixelDeltaU) + (j * pixelDeltaV);
            auto rayDirection = pixelCenter - cameraCenter;
            Ray r(cameraCenter, rayDirection);

            Color pixelColor = rayColor(r);
            
            int ir = static_cast<int>(255.999 * pixelColor.x());
            int ig = static_cast<int>(255.999 * pixelColor.y());
            int ib = static_cast<int>(255.999 * pixelColor.z());

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    std::clog << "\rDone.                 \n";

    return 0;
}
