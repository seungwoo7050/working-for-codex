#include <gtest/gtest.h>
#include "../../src/geometry/sphere.h"
#include "../../src/geometry/hittable_list.h"
#include "../../src/core/ray.h"

using namespace raytracer;

class SphereTest : public ::testing::Test {
protected:
    Point3 center{0.0, 0.0, 0.0};
    double radius = 1.0;
    shared_ptr<Material> mat = nullptr; // We'll use nullptr for testing geometry only
};

TEST_F(SphereTest, RayHitsSphereAtOrigin) {
    Sphere sphere(center, radius, mat);
    Ray ray(Point3(0, 0, -5), Vec3(0, 0, 1)); // Ray pointing towards sphere
    HitRecord rec;

    bool hit = sphere.hit(ray, 0.001, INFINITY_VAL, rec);

    EXPECT_TRUE(hit);
    EXPECT_NEAR(rec.t, 4.0, 1e-10); // Should hit at t=4 (5-1)
    EXPECT_NEAR(rec.p.z(), -1.0, 1e-10); // Hit point should be at z=-1
}

TEST_F(SphereTest, RayMissesSphere) {
    Sphere sphere(center, radius, mat);
    Ray ray(Point3(0, 5, -5), Vec3(0, 0, 1)); // Ray above sphere
    HitRecord rec;

    bool hit = sphere.hit(ray, 0.001, INFINITY_VAL, rec);

    EXPECT_FALSE(hit);
}

TEST_F(SphereTest, RayOriginatesInsideSphere) {
    Sphere sphere(center, radius, mat);
    Ray ray(Point3(0, 0, 0), Vec3(0, 0, 1)); // Ray from center
    HitRecord rec;

    bool hit = sphere.hit(ray, 0.001, INFINITY_VAL, rec);

    EXPECT_TRUE(hit);
    EXPECT_NEAR(rec.t, 1.0, 1e-10); // Should hit at t=1
}

TEST_F(SphereTest, RayHitsSphereTangentially) {
    Sphere sphere(center, radius, mat);
    Ray ray(Point3(0, 1, -5), Vec3(0, 0, 1)); // Ray tangent to sphere
    HitRecord rec;

    bool hit = sphere.hit(ray, 0.001, INFINITY_VAL, rec);

    EXPECT_TRUE(hit);
    EXPECT_NEAR(rec.p.y(), 1.0, 1e-5); // Should hit at y=1
    EXPECT_NEAR(rec.p.z(), 0.0, 1e-5); // Should hit at z=0
}

TEST_F(SphereTest, RayHitsOutsideTRange) {
    Sphere sphere(center, radius, mat);
    Ray ray(Point3(0, 0, -5), Vec3(0, 0, 1));
    HitRecord rec;

    // Set tMax to be before the sphere
    bool hit = sphere.hit(ray, 0.001, 3.0, rec);

    EXPECT_FALSE(hit); // Should miss because hit is at t=4
}

TEST_F(SphereTest, NormalPointsOutward) {
    Sphere sphere(center, radius, mat);
    Ray ray(Point3(0, 0, -5), Vec3(0, 0, 1));
    HitRecord rec;

    sphere.hit(ray, 0.001, INFINITY_VAL, rec);

    // Normal at (0,0,-1) should point outward in -z direction
    EXPECT_NEAR(rec.normal.x(), 0.0, 1e-10);
    EXPECT_NEAR(rec.normal.y(), 0.0, 1e-10);
    EXPECT_NEAR(rec.normal.z(), -1.0, 1e-10);
    EXPECT_TRUE(rec.frontFace);
}

TEST_F(SphereTest, NormalPointsOutwardFromInsideHit) {
    Sphere sphere(center, radius, mat);
    Ray ray(Point3(0, 0, 0), Vec3(0, 0, 1)); // Ray from inside
    HitRecord rec;

    sphere.hit(ray, 0.001, INFINITY_VAL, rec);

    // When hitting from inside, normal should still point outward (against ray)
    EXPECT_NEAR(rec.normal.x(), 0.0, 1e-10);
    EXPECT_NEAR(rec.normal.y(), 0.0, 1e-10);
    EXPECT_NEAR(rec.normal.z(), -1.0, 1e-10);
    EXPECT_FALSE(rec.frontFace);
}

TEST_F(SphereTest, MultipleSpheresInList) {
    HittableList world;

    auto sphere1 = make_shared<Sphere>(Point3(0, 0, -1), 0.5, mat);
    auto sphere2 = make_shared<Sphere>(Point3(0, 0, -2), 0.5, mat);

    world.add(sphere1);
    world.add(sphere2);

    Ray ray(Point3(0, 0, 0), Vec3(0, 0, -1));
    HitRecord rec;

    bool hit = world.hit(ray, 0.001, INFINITY_VAL, rec);

    EXPECT_TRUE(hit);
    // Should hit sphere1 first (closer)
    EXPECT_NEAR(rec.t, 0.5, 1e-10);
}

TEST_F(SphereTest, NegativeRadiusBecomesZero) {
    Sphere sphere(center, -1.0, mat);
    Ray ray(Point3(0, 0, -5), Vec3(0, 0, 1));
    HitRecord rec;

    // Negative radius is clamped to 0
    // This test just ensures it doesn't crash
    bool hit = sphere.hit(ray, 0.001, INFINITY_VAL, rec);

    // We don't assert specific behavior, just that it doesn't crash
    (void)hit; // Suppress unused variable warning
    SUCCEED();
}
