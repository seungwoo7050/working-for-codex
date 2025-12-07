/**
 * Ray Tracer - v1.0.0 Basic Raytracer
 * 
 * Complete basic raytracer with:
 * - Vec3, Ray, Camera
 * - Sphere geometry
 * - Lambertian, Metal, Dielectric materials
 * - Anti-aliasing with multi-sampling
 * - Defocus blur (depth of field)
 */
#include "core/camera.h"
#include "core/color.h"
#include "geometry/hittable_list.h"
#include "geometry/sphere.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/dielectric.h"

using namespace raytracer;

int main() {
    HittableList world;

    // Ground
    auto groundMaterial = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMaterial));

    // Three spheres
    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    // Camera
    Camera cam;
    cam.aspectRatio = 16.0 / 9.0;
    cam.imageWidth = 400;
    cam.samplesPerPixel = 10;
    cam.maxDepth = 50;
    cam.vfov = 20;
    cam.lookfrom = Point3(13, 2, 3);
    cam.lookat = Point3(0, 0, 0);
    cam.vup = Vec3(0, 1, 0);
    cam.defocusAngle = 0.6;
    cam.focusDist = 10.0;

    cam.render(world);

    return 0;
}
