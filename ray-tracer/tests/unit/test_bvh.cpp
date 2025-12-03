#include <gtest/gtest.h>
#include "geometry/bvh_node.h"
#include "geometry/sphere.h"
#include "materials/lambertian.h"
#include "core/color.h"

using namespace raytracer;

TEST(BVHTest, AABBHitTest) {
    AABB box(Point3(0, 0, 0), Point3(1, 1, 1));

    // Ray that should hit the box
    Ray r1(Point3(-1, 0.5, 0.5), Vec3(1, 0, 0));
    EXPECT_TRUE(box.hit(r1, 0.001, 10.0));

    // Ray that should miss the box
    Ray r2(Point3(-1, 2, 2), Vec3(1, 0, 0));
    EXPECT_FALSE(box.hit(r2, 0.001, 10.0));
}

TEST(BVHTest, AABBSurroundingBox) {
    AABB box1(Point3(0, 0, 0), Point3(1, 1, 1));
    AABB box2(Point3(0.5, 0.5, 0.5), Point3(2, 2, 2));

    AABB surrounding = AABB::surroundingBox(box1, box2);

    EXPECT_DOUBLE_EQ(surrounding.min().x(), 0.0);
    EXPECT_DOUBLE_EQ(surrounding.min().y(), 0.0);
    EXPECT_DOUBLE_EQ(surrounding.min().z(), 0.0);
    EXPECT_DOUBLE_EQ(surrounding.max().x(), 2.0);
    EXPECT_DOUBLE_EQ(surrounding.max().y(), 2.0);
    EXPECT_DOUBLE_EQ(surrounding.max().z(), 2.0);
}

TEST(BVHTest, SphereBoundingBox) {
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    Sphere sphere(Point3(0, 0, 0), 1.0, material);

    AABB box;
    EXPECT_TRUE(sphere.boundingBox(0, 1, box));

    EXPECT_DOUBLE_EQ(box.min().x(), -1.0);
    EXPECT_DOUBLE_EQ(box.min().y(), -1.0);
    EXPECT_DOUBLE_EQ(box.min().z(), -1.0);
    EXPECT_DOUBLE_EQ(box.max().x(), 1.0);
    EXPECT_DOUBLE_EQ(box.max().y(), 1.0);
    EXPECT_DOUBLE_EQ(box.max().z(), 1.0);
}

TEST(BVHTest, BVHConstruction) {
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));

    HittableList world;
    world.add(make_shared<Sphere>(Point3(0, 0, 0), 1.0, material));
    world.add(make_shared<Sphere>(Point3(3, 0, 0), 1.0, material));
    world.add(make_shared<Sphere>(Point3(0, 3, 0), 1.0, material));

    // Should not throw
    BVHNode bvh(world, 0, 1);

    AABB box;
    EXPECT_TRUE(bvh.boundingBox(0, 1, box));
}

TEST(BVHTest, BVHHitTest) {
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));

    HittableList world;
    world.add(make_shared<Sphere>(Point3(0, 0, -5), 1.0, material));
    world.add(make_shared<Sphere>(Point3(2, 0, -5), 1.0, material));

    BVHNode bvh(world, 0, 1);

    // Ray that should hit one of the spheres
    Ray r(Point3(0, 0, 0), Vec3(0, 0, -1));
    HitRecord rec;
    EXPECT_TRUE(bvh.hit(r, 0.001, 100.0, rec));

    // Ray that should miss
    Ray r2(Point3(10, 10, 0), Vec3(0, 0, -1));
    EXPECT_FALSE(bvh.hit(r2, 0.001, 100.0, rec));
}
