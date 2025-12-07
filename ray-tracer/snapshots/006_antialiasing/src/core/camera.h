#pragma once

#include "color.h"
#include "ray.h"
#include "../geometry/hittable.h"
#include "../geometry/hittable_list.h"
#include "../materials/material.h"
#include "../utils/common.h"
#include <iostream>

namespace raytracer {

class Camera {
public:
    double aspectRatio = 16.0 / 9.0;
    int imageWidth = 400;
    int samplesPerPixel = 10;
    int maxDepth = 10;
    double vfov = 90;
    Point3 lookfrom = Point3(0, 0, 0);
    Point3 lookat = Point3(0, 0, -1);
    Vec3 vup = Vec3(0, 1, 0);
    double defocusAngle = 0;
    double focusDist = 10;

    void render(const Hittable& world) {
        initialize();

        std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

        for (int j = 0; j < imageHeight; j++) {
            std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
            for (int i = 0; i < imageWidth; i++) {
                Color pixelColor(0, 0, 0);
                for (int sample = 0; sample < samplesPerPixel; sample++) {
                    Ray r = getRay(i, j);
                    pixelColor += rayColor(r, maxDepth, world);
                }
                writeColor(std::cout, pixelSamplesScale * pixelColor);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:
    int imageHeight;
    double pixelSamplesScale;
    Point3 center;
    Point3 pixel00Loc;
    Vec3 pixelDeltaU;
    Vec3 pixelDeltaV;
    Vec3 u, v, w;
    Vec3 defocusDiskU;
    Vec3 defocusDiskV;

    void initialize() {
        imageHeight = static_cast<int>(imageWidth / aspectRatio);
        imageHeight = (imageHeight < 1) ? 1 : imageHeight;

        pixelSamplesScale = 1.0 / samplesPerPixel;

        center = lookfrom;

        auto theta = degreesToRadians(vfov);
        auto h = std::tan(theta / 2);
        auto viewportHeight = 2 * h * focusDist;
        auto viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / imageHeight);

        w = unitVector(lookfrom - lookat);
        u = unitVector(cross(vup, w));
        v = cross(w, u);

        Vec3 viewportU = viewportWidth * u;
        Vec3 viewportV = viewportHeight * -v;

        pixelDeltaU = viewportU / imageWidth;
        pixelDeltaV = viewportV / imageHeight;

        auto viewportUpperLeft = center - (focusDist * w) - viewportU / 2 - viewportV / 2;
        pixel00Loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

        auto defocusRadius = focusDist * std::tan(degreesToRadians(defocusAngle / 2));
        defocusDiskU = u * defocusRadius;
        defocusDiskV = v * defocusRadius;
    }

    Ray getRay(int i, int j) const {
        auto offset = sampleSquare();
        auto pixelSample = pixel00Loc + ((i + offset.x()) * pixelDeltaU)
                                       + ((j + offset.y()) * pixelDeltaV);

        auto rayOrigin = (defocusAngle <= 0) ? center : defocusDiskSample();
        auto rayDirection = pixelSample - rayOrigin;

        return Ray(rayOrigin, rayDirection);
    }

    Vec3 sampleSquare() const {
        return Vec3(randomDouble() - 0.5, randomDouble() - 0.5, 0);
    }

    Point3 defocusDiskSample() const {
        auto p = randomInUnitDisk();
        return center + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
    }

    Color rayColor(const Ray& r, int depth, const Hittable& world) const {
        if (depth <= 0)
            return Color(0, 0, 0);

        HitRecord rec;

        if (!world.hit(r, 0.001, INFINITY_VAL, rec)) {
            // Background gradient
            Vec3 unitDirection = unitVector(r.direction());
            auto a = 0.5 * (unitDirection.y() + 1.0);
            return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
        }

        Ray scattered;
        Color attenuation;

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return Color(0, 0, 0);

        return attenuation * rayColor(scattered, depth - 1, world);
    }
};

} // namespace raytracer
