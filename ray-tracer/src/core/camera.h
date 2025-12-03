#pragma once

#include "color.h"
#include "ray.h"
#include "../geometry/hittable.h"
#include "../geometry/hittable_list.h"
#include "../materials/material.h"
#include "../utils/common.h"
#include "../utils/pdf.h"
#include <iostream>

namespace raytracer {

class Camera {
public:
    double aspectRatio = 16.0 / 9.0;  // Ratio of image width over height
    int imageWidth = 400;              // Rendered image width in pixel count
    int samplesPerPixel = 10;          // Count of random samples for each pixel
    int maxDepth = 10;                 // Maximum number of ray bounces into scene
    double vfov = 90;                  // Vertical view angle (field of view)
    Point3 lookfrom = Point3(0, 0, 0); // Point camera is looking from
    Point3 lookat = Point3(0, 0, -1);  // Point camera is looking at
    Vec3 vup = Vec3(0, 1, 0);          // Camera-relative "up" direction
    double defocusAngle = 0;           // Variation angle of rays through each pixel
    double focusDist = 10;             // Distance from camera lookfrom point to plane of perfect focus
    Color background;                  // Background color

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

    void render(const Hittable& world, const Hittable& lights) {
        initialize();

        std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

        for (int j = 0; j < imageHeight; j++) {
            std::clog << "\rScanlines remaining: " << (imageHeight - j) << ' ' << std::flush;
            for (int i = 0; i < imageWidth; i++) {
                Color pixelColor(0, 0, 0);
                for (int sample = 0; sample < samplesPerPixel; sample++) {
                    Ray r = getRay(i, j);
                    pixelColor += rayColor(r, maxDepth, world, lights);
                }
                writeColor(std::cout, pixelSamplesScale * pixelColor);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:
    int imageHeight;           // Rendered image height
    double pixelSamplesScale;  // Color scale factor for a sum of pixel samples
    Point3 center;             // Camera center
    Point3 pixel00Loc;         // Location of pixel 0, 0
    Vec3 pixelDeltaU;          // Offset to pixel to the right
    Vec3 pixelDeltaV;          // Offset to pixel below
    Vec3 u, v, w;              // Camera frame basis vectors
    Vec3 defocusDiskU;         // Defocus disk horizontal radius
    Vec3 defocusDiskV;         // Defocus disk vertical radius

    void initialize() {
        imageHeight = static_cast<int>(imageWidth / aspectRatio);
        imageHeight = (imageHeight < 1) ? 1 : imageHeight;

        pixelSamplesScale = 1.0 / samplesPerPixel;

        center = lookfrom;

        // Determine viewport dimensions
        auto theta = degreesToRadians(vfov);
        auto h = std::tan(theta / 2);
        auto viewportHeight = 2 * h * focusDist;
        auto viewportWidth = viewportHeight * (static_cast<double>(imageWidth) / imageHeight);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame
        w = unitVector(lookfrom - lookat);
        u = unitVector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges
        Vec3 viewportU = viewportWidth * u;    // Vector across viewport horizontal edge
        Vec3 viewportV = viewportHeight * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel
        pixelDeltaU = viewportU / imageWidth;
        pixelDeltaV = viewportV / imageHeight;

        // Calculate the location of the upper left pixel
        auto viewportUpperLeft = center - (focusDist * w) - viewportU / 2 - viewportV / 2;
        pixel00Loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

        // Calculate the camera defocus disk basis vectors
        auto defocusRadius = focusDist * std::tan(degreesToRadians(defocusAngle / 2));
        defocusDiskU = u * defocusRadius;
        defocusDiskV = v * defocusRadius;
    }

    Ray getRay(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        auto offset = sampleSquare();
        auto pixelSample = pixel00Loc + ((i + offset.x()) * pixelDeltaU)
                                       + ((j + offset.y()) * pixelDeltaV);

        auto rayOrigin = (defocusAngle <= 0) ? center : defocusDiskSample();
        auto rayDirection = pixelSample - rayOrigin;
        auto rayTime = randomDouble();

        return Ray(rayOrigin, rayDirection, rayTime);
    }

    Vec3 sampleSquare() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return Vec3(randomDouble() - 0.5, randomDouble() - 0.5, 0);
    }

    Point3 defocusDiskSample() const {
        // Returns a random point in the camera defocus disk.
        auto p = randomInUnitDisk();
        return center + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
    }

    Color rayColor(const Ray& r, int depth, const Hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered
        if (depth <= 0)
            return Color(0, 0, 0);

        HitRecord rec;

        // If the ray hits nothing, return the background color
        if (!world.hit(r, 0.001, INFINITY_VAL, rec))
            return background;

        Ray scattered;
        Color attenuation;
        Color colorFromEmission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return colorFromEmission;

        Color colorFromScatter = attenuation * rayColor(scattered, depth - 1, world);

        return colorFromEmission + colorFromScatter;
    }

    Color rayColor(const Ray& r, int depth, const Hittable& world, const Hittable& lights) const {
        // If we've exceeded the ray bounce limit, no more light is gathered
        if (depth <= 0)
            return Color(0, 0, 0);

        HitRecord rec;

        // If the ray hits nothing, return the background color
        if (!world.hit(r, 0.001, INFINITY_VAL, rec))
            return background;

        ScatterRecord srec;
        Color colorFromEmission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, srec))
            return colorFromEmission;

        if (srec.skipPdf) {
            return srec.attenuation * rayColor(srec.skipPdfRay, depth - 1, world, lights);
        }

        auto lightPtr = make_shared<HittablePDF>(lights, rec.p);
        MixturePDF mixedPdf(lightPtr, srec.pdfPtr);

        Ray scattered = Ray(rec.p, mixedPdf.generate(), r.time());
        auto pdfValue = mixedPdf.value(scattered.direction());

        double scatteringPdf = rec.mat->scatteringPdf(r, rec, scattered);

        Color colorFromScatter = (srec.attenuation * scatteringPdf * rayColor(scattered, depth - 1, world, lights)) / pdfValue;

        return colorFromEmission + colorFromScatter;
    }
};

} // namespace raytracer
