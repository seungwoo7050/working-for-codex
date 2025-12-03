#include <gtest/gtest.h>
#include "../../src/core/ray.h"

using namespace raytracer;

class RayTest : public ::testing::Test {
protected:
    Point3 origin{1.0, 2.0, 3.0};
    Vec3 direction{4.0, 5.0, 6.0};
    Ray ray{origin, direction};
};

TEST_F(RayTest, DefaultConstructor) {
    Ray r;
    // Default ray should exist (though values may be undefined)
    // This test just ensures the default constructor compiles
}

TEST_F(RayTest, ParameterizedConstructor) {
    EXPECT_DOUBLE_EQ(ray.origin().x(), 1.0);
    EXPECT_DOUBLE_EQ(ray.origin().y(), 2.0);
    EXPECT_DOUBLE_EQ(ray.origin().z(), 3.0);
    EXPECT_DOUBLE_EQ(ray.direction().x(), 4.0);
    EXPECT_DOUBLE_EQ(ray.direction().y(), 5.0);
    EXPECT_DOUBLE_EQ(ray.direction().z(), 6.0);
}

TEST_F(RayTest, AtParameterZero) {
    Point3 p = ray.at(0.0);
    EXPECT_DOUBLE_EQ(p.x(), origin.x());
    EXPECT_DOUBLE_EQ(p.y(), origin.y());
    EXPECT_DOUBLE_EQ(p.z(), origin.z());
}

TEST_F(RayTest, AtParameterOne) {
    Point3 p = ray.at(1.0);
    EXPECT_DOUBLE_EQ(p.x(), 5.0); // 1 + 4
    EXPECT_DOUBLE_EQ(p.y(), 7.0); // 2 + 5
    EXPECT_DOUBLE_EQ(p.z(), 9.0); // 3 + 6
}

TEST_F(RayTest, AtParameterTwo) {
    Point3 p = ray.at(2.0);
    EXPECT_DOUBLE_EQ(p.x(), 9.0);  // 1 + 2*4
    EXPECT_DOUBLE_EQ(p.y(), 12.0); // 2 + 2*5
    EXPECT_DOUBLE_EQ(p.z(), 15.0); // 3 + 2*6
}

TEST_F(RayTest, AtParameterNegative) {
    Point3 p = ray.at(-1.0);
    EXPECT_DOUBLE_EQ(p.x(), -3.0); // 1 - 4
    EXPECT_DOUBLE_EQ(p.y(), -3.0); // 2 - 5
    EXPECT_DOUBLE_EQ(p.z(), -3.0); // 3 - 6
}

TEST_F(RayTest, AtParameterFractional) {
    Point3 p = ray.at(0.5);
    EXPECT_DOUBLE_EQ(p.x(), 3.0); // 1 + 0.5*4
    EXPECT_DOUBLE_EQ(p.y(), 4.5); // 2 + 0.5*5
    EXPECT_DOUBLE_EQ(p.z(), 6.0); // 3 + 0.5*6
}
