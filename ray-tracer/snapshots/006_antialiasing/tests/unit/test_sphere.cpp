#include <gtest/gtest.h>
#include "geometry/sphere.h"
#include "materials/lambertian.h"

using namespace raytracer;

TEST(SphereTest, RayHitsSphere) {
    auto mat = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    Sphere s(Point3(0, 0, -1), 0.5, mat);
    Ray r(Point3(0, 0, 0), Vec3(0, 0, -1));
    HitRecord rec;
    
    EXPECT_TRUE(s.hit(r, 0.001, INFINITY_VAL, rec));
    EXPECT_NEAR(rec.t, 0.5, 0.001);
}

TEST(SphereTest, RayMissesSphere) {
    auto mat = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    Sphere s(Point3(0, 0, -1), 0.5, mat);
    Ray r(Point3(0, 0, 0), Vec3(0, 1, 0));  // Pointing up, misses sphere
    HitRecord rec;
    
    EXPECT_FALSE(s.hit(r, 0.001, INFINITY_VAL, rec));
}

TEST(SphereTest, NormalPointsOutward) {
    auto mat = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    Sphere s(Point3(0, 0, -1), 0.5, mat);
    Ray r(Point3(0, 0, 0), Vec3(0, 0, -1));
    HitRecord rec;
    
    s.hit(r, 0.001, INFINITY_VAL, rec);
    
    // Normal should point toward camera (positive z)
    EXPECT_TRUE(rec.frontFace);
    EXPECT_GT(rec.normal.z(), 0);
}
