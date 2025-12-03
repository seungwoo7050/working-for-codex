#include "core/camera.h"
#include "core/color.h"
#include "geometry/hittable_list.h"
#include "geometry/sphere.h"
#include "geometry/bvh_node.h"
#include "geometry/quad.h"
#include "geometry/moving_sphere.h"
#include "geometry/translate.h"
#include "geometry/rotate_y.h"
#include "geometry/constant_medium.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/dielectric.h"
#include "materials/diffuse_light.h"
#include "materials/isotropic.h"
#include "textures/checker_texture.h"
#include "textures/noise_texture.h"

using namespace raytracer;

// Create a box (6 quads)
shared_ptr<HittableList> box(const Point3& a, const Point3& b, shared_ptr<Material> mat) {
    auto sides = make_shared<HittableList>();

    auto min = Point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = Point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = Vec3(max.x() - min.x(), 0, 0);
    auto dy = Vec3(0, max.y() - min.y(), 0);
    auto dz = Vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<Quad>(Point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(make_shared<Quad>(Point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(make_shared<Quad>(Point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<Quad>(Point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<Quad>(Point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<Quad>(Point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

HittableList randomSpheres() {
    HittableList world;

    auto checker = make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));
    world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto chooseMat = randomDouble();
            Point3 center(a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble());

            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphereMaterial;

                if (chooseMat < 0.8) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphereMaterial = make_shared<Lambertian>(albedo);
                    auto center2 = center + Vec3(0, randomDouble(0, .5), 0);
                    world.add(make_shared<MovingSphere>(
                        center, center2, 0.0, 1.0, 0.2, sphereMaterial));
                } else if (chooseMat < 0.95) {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = randomDouble(0, 0.5);
                    sphereMaterial = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphereMaterial));
                } else {
                    // glass
                    sphereMaterial = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}

HittableList twoSpheres() {
    HittableList world;

    auto checker = make_shared<CheckerTexture>(0.8, Color(.2, .3, .1), Color(.9, .9, .9));

    world.add(make_shared<Sphere>(Point3(0, -10, 0), 10, make_shared<Lambertian>(checker)));
    world.add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<Lambertian>(checker)));

    return world;
}

HittableList earth() {
    HittableList world;

    auto earthTexture = make_shared<NoiseTexture>(4);
    auto earthSurface = make_shared<Lambertian>(earthTexture);
    auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2, earthSurface);

    world.add(globe);

    return world;
}

HittableList simpleLight() {
    HittableList world;

    auto pertext = make_shared<NoiseTexture>(4);
    world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
    world.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

    auto difflight = make_shared<DiffuseLight>(Color(4, 4, 4));
    world.add(make_shared<Sphere>(Point3(0, 7, 0), 2, difflight));
    world.add(make_shared<Quad>(Point3(3, 1, -2), Vec3(2, 0, 0), Vec3(0, 2, 0), difflight));

    return world;
}

HittableList cornellBox() {
    HittableList world;

    auto red = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

    // Cornell box walls
    world.add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    world.add(make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    world.add(make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
    world.add(make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    world.add(make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
    world.add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    // Boxes using box function
    shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
    world.add(box1);

    shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
    world.add(box2);

    return world;
}

HittableList cornellSmoke() {
    HittableList world;

    auto red = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));

    world.add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    world.add(make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    world.add(make_shared<Quad>(Point3(113, 554, 127), Vec3(330, 0, 0), Vec3(0, 0, 305), light));
    world.add(make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    world.add(make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    world.add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    shared_ptr<Hittable> box1 = make_shared<Quad>(Point3(0, 0, 0), Vec3(165, 0, 0), Vec3(0, 165, 0), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));

    shared_ptr<Hittable> box2 = make_shared<Quad>(Point3(0, 0, 0), Vec3(165, 0, 0), Vec3(0, 330, 0), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));

    world.add(make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
    world.add(make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));

    return world;
}

int main() {
    HittableList world;
    Camera cam;

    // Choose scene
    int sceneChoice = 5;  // Change this to select different scenes (5 = Cornell Box with importance sampling)

    switch (sceneChoice) {
        case 1:
            world = randomSpheres();
            cam.aspectRatio = 16.0 / 9.0;
            cam.imageWidth = 400;
            cam.samplesPerPixel = 10;
            cam.maxDepth = 50;
            cam.background = Color(0.70, 0.80, 1.00);
            cam.vfov = 20;
            cam.lookfrom = Point3(13, 2, 3);
            cam.lookat = Point3(0, 0, 0);
            cam.vup = Vec3(0, 1, 0);
            cam.defocusAngle = 0.6;
            cam.focusDist = 10.0;
            break;

        case 2:
            world = twoSpheres();
            cam.aspectRatio = 16.0 / 9.0;
            cam.imageWidth = 400;
            cam.samplesPerPixel = 10;
            cam.maxDepth = 50;
            cam.background = Color(0.70, 0.80, 1.00);
            cam.vfov = 20;
            cam.lookfrom = Point3(13, 2, 3);
            cam.lookat = Point3(0, 0, 0);
            cam.vup = Vec3(0, 1, 0);
            cam.defocusAngle = 0;
            break;

        case 3:
            world = earth();
            cam.aspectRatio = 16.0 / 9.0;
            cam.imageWidth = 400;
            cam.samplesPerPixel = 10;
            cam.maxDepth = 50;
            cam.background = Color(0.70, 0.80, 1.00);
            cam.vfov = 20;
            cam.lookfrom = Point3(0, 0, 12);
            cam.lookat = Point3(0, 0, 0);
            cam.vup = Vec3(0, 1, 0);
            cam.defocusAngle = 0;
            break;

        case 4:
            world = simpleLight();
            cam.aspectRatio = 16.0 / 9.0;
            cam.imageWidth = 400;
            cam.samplesPerPixel = 10;
            cam.maxDepth = 50;
            cam.background = Color(0, 0, 0);
            cam.vfov = 20;
            cam.lookfrom = Point3(26, 3, 6);
            cam.lookat = Point3(0, 2, 0);
            cam.vup = Vec3(0, 1, 0);
            cam.defocusAngle = 0;
            break;

        case 5: {
            // Cornell Box with importance sampling
            world = cornellBox();
            cam.aspectRatio = 1.0;
            cam.imageWidth = 600;
            cam.samplesPerPixel = 200;
            cam.maxDepth = 50;
            cam.background = Color(0, 0, 0);
            cam.vfov = 40;
            cam.lookfrom = Point3(278, 278, -800);
            cam.lookat = Point3(278, 278, 0);
            cam.vup = Vec3(0, 1, 0);
            cam.defocusAngle = 0;

            // Light for importance sampling
            auto light = make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), shared_ptr<Material>());

            // Use BVH for acceleration
            BVHNode bvh(world, 0, 1);

            // Render with importance sampling
            cam.render(bvh, *light);

            return 0;
        }

        case 6:
            world = cornellSmoke();
            cam.aspectRatio = 1.0;
            cam.imageWidth = 400;
            cam.samplesPerPixel = 10;
            cam.maxDepth = 50;
            cam.background = Color(0, 0, 0);
            cam.vfov = 40;
            cam.lookfrom = Point3(278, 278, -800);
            cam.lookat = Point3(278, 278, 0);
            cam.vup = Vec3(0, 1, 0);
            cam.defocusAngle = 0;
            break;

        default:
            world = randomSpheres();
            cam.background = Color(0.70, 0.80, 1.00);
    }

    // Use BVH for acceleration
    BVHNode bvh(world, 0, 1);

    // Render
    cam.render(bvh);

    return 0;
}
