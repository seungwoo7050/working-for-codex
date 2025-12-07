#include <gtest/gtest.h>
#include "core/ray.h"

using namespace raytracer;

TEST(RayTest, DefaultConstructor) {
    Ray r;
    // Default constructed ray has undefined state, just verify it compiles
}

TEST(RayTest, ParameterizedConstructor) {
    Point3 origin(1.0, 2.0, 3.0);
    Vec3 direction(0.0, 0.0, -1.0);
    Ray r(origin, direction);
    
    EXPECT_DOUBLE_EQ(r.origin().x(), 1.0);
    EXPECT_DOUBLE_EQ(r.origin().y(), 2.0);
    EXPECT_DOUBLE_EQ(r.origin().z(), 3.0);
    
    EXPECT_DOUBLE_EQ(r.direction().x(), 0.0);
    EXPECT_DOUBLE_EQ(r.direction().y(), 0.0);
    EXPECT_DOUBLE_EQ(r.direction().z(), -1.0);
}

TEST(RayTest, AtFunction) {
    Point3 origin(0.0, 0.0, 0.0);
    Vec3 direction(1.0, 2.0, 3.0);
    Ray r(origin, direction);
    
    Point3 p = r.at(2.0);
    EXPECT_DOUBLE_EQ(p.x(), 2.0);
    EXPECT_DOUBLE_EQ(p.y(), 4.0);
    EXPECT_DOUBLE_EQ(p.z(), 6.0);
}

TEST(RayTest, AtFunctionWithOffset) {
    Point3 origin(1.0, 1.0, 1.0);
    Vec3 direction(2.0, 0.0, 0.0);
    Ray r(origin, direction);
    
    Point3 p = r.at(0.5);
    EXPECT_DOUBLE_EQ(p.x(), 2.0);
    EXPECT_DOUBLE_EQ(p.y(), 1.0);
    EXPECT_DOUBLE_EQ(p.z(), 1.0);
}
