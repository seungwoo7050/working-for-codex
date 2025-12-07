#include <gtest/gtest.h>
#include "core/vec3.h"

using namespace raytracer;

TEST(Vec3Test, DefaultConstructor) {
    Vec3 v;
    EXPECT_DOUBLE_EQ(v.x(), 0.0);
    EXPECT_DOUBLE_EQ(v.y(), 0.0);
    EXPECT_DOUBLE_EQ(v.z(), 0.0);
}

TEST(Vec3Test, DotProduct) {
    Vec3 u(1.0, 2.0, 3.0);
    Vec3 v(4.0, 5.0, 6.0);
    EXPECT_DOUBLE_EQ(dot(u, v), 32.0);
}

TEST(Vec3Test, CrossProduct) {
    Vec3 u(1.0, 0.0, 0.0);
    Vec3 v(0.0, 1.0, 0.0);
    Vec3 result = cross(u, v);
    EXPECT_DOUBLE_EQ(result.z(), 1.0);
}

TEST(Vec3Test, Length) {
    Vec3 v(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v.length(), 5.0);
}

TEST(Vec3Test, UnitVector) {
    Vec3 v(3.0, 0.0, 0.0);
    Vec3 unit = unitVector(v);
    EXPECT_DOUBLE_EQ(unit.x(), 1.0);
}
