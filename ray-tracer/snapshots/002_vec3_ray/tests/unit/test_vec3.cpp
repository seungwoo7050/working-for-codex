#include <gtest/gtest.h>
#include "core/vec3.h"

using namespace raytracer;

TEST(Vec3Test, DefaultConstructor) {
    Vec3 v;
    EXPECT_DOUBLE_EQ(v.x(), 0.0);
    EXPECT_DOUBLE_EQ(v.y(), 0.0);
    EXPECT_DOUBLE_EQ(v.z(), 0.0);
}

TEST(Vec3Test, ParameterizedConstructor) {
    Vec3 v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.x(), 1.0);
    EXPECT_DOUBLE_EQ(v.y(), 2.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}

TEST(Vec3Test, Negation) {
    Vec3 v(1.0, -2.0, 3.0);
    Vec3 neg = -v;
    EXPECT_DOUBLE_EQ(neg.x(), -1.0);
    EXPECT_DOUBLE_EQ(neg.y(), 2.0);
    EXPECT_DOUBLE_EQ(neg.z(), -3.0);
}

TEST(Vec3Test, Addition) {
    Vec3 u(1.0, 2.0, 3.0);
    Vec3 v(4.0, 5.0, 6.0);
    Vec3 result = u + v;
    EXPECT_DOUBLE_EQ(result.x(), 5.0);
    EXPECT_DOUBLE_EQ(result.y(), 7.0);
    EXPECT_DOUBLE_EQ(result.z(), 9.0);
}

TEST(Vec3Test, Subtraction) {
    Vec3 u(4.0, 5.0, 6.0);
    Vec3 v(1.0, 2.0, 3.0);
    Vec3 result = u - v;
    EXPECT_DOUBLE_EQ(result.x(), 3.0);
    EXPECT_DOUBLE_EQ(result.y(), 3.0);
    EXPECT_DOUBLE_EQ(result.z(), 3.0);
}

TEST(Vec3Test, DotProduct) {
    Vec3 u(1.0, 2.0, 3.0);
    Vec3 v(4.0, 5.0, 6.0);
    EXPECT_DOUBLE_EQ(dot(u, v), 32.0);  // 1*4 + 2*5 + 3*6 = 32
}

TEST(Vec3Test, CrossProduct) {
    Vec3 u(1.0, 0.0, 0.0);
    Vec3 v(0.0, 1.0, 0.0);
    Vec3 result = cross(u, v);
    EXPECT_DOUBLE_EQ(result.x(), 0.0);
    EXPECT_DOUBLE_EQ(result.y(), 0.0);
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
    EXPECT_DOUBLE_EQ(unit.y(), 0.0);
    EXPECT_DOUBLE_EQ(unit.z(), 0.0);
}

TEST(Vec3Test, NearZero) {
    Vec3 v(1e-9, 1e-9, 1e-9);
    EXPECT_TRUE(v.nearZero());
    
    Vec3 v2(1.0, 0.0, 0.0);
    EXPECT_FALSE(v2.nearZero());
}
