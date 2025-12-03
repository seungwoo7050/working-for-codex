#include <gtest/gtest.h>
#include "geometry/quad.h"
#include "geometry/sphere.h"
#include "geometry/moving_sphere.h"
#include "geometry/translate.h"
#include "geometry/rotate_y.h"
#include "materials/lambertian.h"
#include "core/color.h"

using namespace raytracer;

TEST(GeometryTest, QuadHitTest) {
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    Quad quad(Point3(0, 0, -5), Vec3(2, 0, 0), Vec3(0, 2, 0), material);

    // Ray that should hit the quad
    Ray r1(Point3(0, 0, 0), Vec3(0, 0, -1));
    HitRecord rec;
    EXPECT_TRUE(quad.hit(r1, 0.001, 100.0, rec));

    // Ray that should miss the quad
    Ray r2(Point3(10, 10, 0), Vec3(0, 0, -1));
    EXPECT_FALSE(quad.hit(r2, 0.001, 100.0, rec));
}

TEST(GeometryTest, MovingSphereHitTest) {
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    MovingSphere sphere(
        Point3(0, 0, -5), Point3(1, 0, -5),
        0.0, 1.0,
        1.0, material
    );

    // Ray at time 0 should hit at original position
    Ray r1(Point3(0, 0, 0), Vec3(0, 0, -1), 0.0);
    HitRecord rec;
    EXPECT_TRUE(sphere.hit(r1, 0.001, 100.0, rec));

    // Ray at time 1 should hit at new position
    Ray r2(Point3(1, 0, 0), Vec3(0, 0, -1), 1.0);
    EXPECT_TRUE(sphere.hit(r2, 0.001, 100.0, rec));
}

TEST(GeometryTest, TranslateTest) {
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    auto sphere = make_shared<Sphere>(Point3(0, 0, -5), 1.0, material);
    Translate translated(sphere, Vec3(2, 0, 0));

    // Ray should hit the translated sphere
    Ray r(Point3(2, 0, 0), Vec3(0, 0, -1));
    HitRecord rec;
    EXPECT_TRUE(translated.hit(r, 0.001, 100.0, rec));
}

TEST(GeometryTest, RotateYTest) {
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    auto sphere = make_shared<Sphere>(Point3(2, 0, 0), 1.0, material);
    RotateY rotated(sphere, 90);  // 90 degrees around Y axis

    // After rotation, sphere should be at approximately (0, 0, -2)
    AABB box;
    EXPECT_TRUE(rotated.boundingBox(0, 1, box));
}
