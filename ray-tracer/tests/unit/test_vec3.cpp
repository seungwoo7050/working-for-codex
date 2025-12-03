#include <gtest/gtest.h>
#include "../../src/core/vec3.h"

using namespace raytracer;

class Vec3Test : public ::testing::Test {
protected:
    Vec3 v1{1.0, 2.0, 3.0};
    Vec3 v2{4.0, 5.0, 6.0};
    Vec3 zero{0.0, 0.0, 0.0};
};

// Constructor and accessor tests
TEST_F(Vec3Test, DefaultConstructor) {
    Vec3 v;
    EXPECT_DOUBLE_EQ(v.x(), 0.0);
    EXPECT_DOUBLE_EQ(v.y(), 0.0);
    EXPECT_DOUBLE_EQ(v.z(), 0.0);
}

TEST_F(Vec3Test, ParameterizedConstructor) {
    EXPECT_DOUBLE_EQ(v1.x(), 1.0);
    EXPECT_DOUBLE_EQ(v1.y(), 2.0);
    EXPECT_DOUBLE_EQ(v1.z(), 3.0);
}

TEST_F(Vec3Test, ArrayAccessor) {
    EXPECT_DOUBLE_EQ(v1[0], 1.0);
    EXPECT_DOUBLE_EQ(v1[1], 2.0);
    EXPECT_DOUBLE_EQ(v1[2], 3.0);
}

// Unary operators
TEST_F(Vec3Test, UnaryMinus) {
    Vec3 v = -v1;
    EXPECT_DOUBLE_EQ(v.x(), -1.0);
    EXPECT_DOUBLE_EQ(v.y(), -2.0);
    EXPECT_DOUBLE_EQ(v.z(), -3.0);
}

// Addition tests
TEST_F(Vec3Test, Addition) {
    Vec3 v = v1 + v2;
    EXPECT_DOUBLE_EQ(v.x(), 5.0);
    EXPECT_DOUBLE_EQ(v.y(), 7.0);
    EXPECT_DOUBLE_EQ(v.z(), 9.0);
}

TEST_F(Vec3Test, AdditionAssignment) {
    Vec3 v = v1;
    v += v2;
    EXPECT_DOUBLE_EQ(v.x(), 5.0);
    EXPECT_DOUBLE_EQ(v.y(), 7.0);
    EXPECT_DOUBLE_EQ(v.z(), 9.0);
}

// Subtraction tests
TEST_F(Vec3Test, Subtraction) {
    Vec3 v = v2 - v1;
    EXPECT_DOUBLE_EQ(v.x(), 3.0);
    EXPECT_DOUBLE_EQ(v.y(), 3.0);
    EXPECT_DOUBLE_EQ(v.z(), 3.0);
}

// Multiplication tests
TEST_F(Vec3Test, ScalarMultiplication) {
    Vec3 v = 2.0 * v1;
    EXPECT_DOUBLE_EQ(v.x(), 2.0);
    EXPECT_DOUBLE_EQ(v.y(), 4.0);
    EXPECT_DOUBLE_EQ(v.z(), 6.0);
}

TEST_F(Vec3Test, VectorMultiplication) {
    Vec3 v = v1 * v2;
    EXPECT_DOUBLE_EQ(v.x(), 4.0);
    EXPECT_DOUBLE_EQ(v.y(), 10.0);
    EXPECT_DOUBLE_EQ(v.z(), 18.0);
}

TEST_F(Vec3Test, MultiplicationAssignment) {
    Vec3 v = v1;
    v *= 2.0;
    EXPECT_DOUBLE_EQ(v.x(), 2.0);
    EXPECT_DOUBLE_EQ(v.y(), 4.0);
    EXPECT_DOUBLE_EQ(v.z(), 6.0);
}

// Division tests
TEST_F(Vec3Test, ScalarDivision) {
    Vec3 v = v1 / 2.0;
    EXPECT_DOUBLE_EQ(v.x(), 0.5);
    EXPECT_DOUBLE_EQ(v.y(), 1.0);
    EXPECT_DOUBLE_EQ(v.z(), 1.5);
}

TEST_F(Vec3Test, DivisionAssignment) {
    Vec3 v = v1;
    v /= 2.0;
    EXPECT_DOUBLE_EQ(v.x(), 0.5);
    EXPECT_DOUBLE_EQ(v.y(), 1.0);
    EXPECT_DOUBLE_EQ(v.z(), 1.5);
}

// Length tests
TEST_F(Vec3Test, Length) {
    Vec3 v(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v.length(), 5.0);
}

TEST_F(Vec3Test, LengthSquared) {
    Vec3 v(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v.lengthSquared(), 25.0);
}

// Dot product test
TEST_F(Vec3Test, DotProduct) {
    double result = dot(v1, v2);
    EXPECT_DOUBLE_EQ(result, 32.0); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
}

// Cross product test
TEST_F(Vec3Test, CrossProduct) {
    Vec3 i(1, 0, 0);
    Vec3 j(0, 1, 0);
    Vec3 k = cross(i, j);
    EXPECT_DOUBLE_EQ(k.x(), 0.0);
    EXPECT_DOUBLE_EQ(k.y(), 0.0);
    EXPECT_DOUBLE_EQ(k.z(), 1.0);
}

// Unit vector test
TEST_F(Vec3Test, UnitVector) {
    Vec3 v(3.0, 4.0, 0.0);
    Vec3 unit = unitVector(v);
    EXPECT_NEAR(unit.x(), 0.6, 1e-10);
    EXPECT_NEAR(unit.y(), 0.8, 1e-10);
    EXPECT_NEAR(unit.z(), 0.0, 1e-10);
    EXPECT_NEAR(unit.length(), 1.0, 1e-10);
}

// Near zero test
TEST_F(Vec3Test, NearZero) {
    Vec3 v1(1e-9, 1e-9, 1e-9);
    Vec3 v2(1e-7, 0, 0);
    EXPECT_TRUE(v1.nearZero());
    EXPECT_FALSE(v2.nearZero());
}

// Reflect test
TEST_F(Vec3Test, Reflect) {
    Vec3 v(1, -1, 0);
    Vec3 n(0, 1, 0);
    Vec3 r = reflect(v, n);
    EXPECT_DOUBLE_EQ(r.x(), 1.0);
    EXPECT_DOUBLE_EQ(r.y(), 1.0);
    EXPECT_DOUBLE_EQ(r.z(), 0.0);
}

// Refract test
TEST_F(Vec3Test, Refract) {
    Vec3 uv(1, -1, 0);
    uv = unitVector(uv);
    Vec3 n(0, 1, 0);
    double etaiOverEtat = 1.0 / 1.5; // Air to glass
    Vec3 refracted = refract(uv, n, etaiOverEtat);
    // Just check that the result is a valid vector
    EXPECT_FALSE(std::isnan(refracted.x()));
    EXPECT_FALSE(std::isnan(refracted.y()));
    EXPECT_FALSE(std::isnan(refracted.z()));
}

// Random vector tests
TEST_F(Vec3Test, RandomVectorInRange) {
    for (int i = 0; i < 100; i++) {
        Vec3 v = Vec3::random(0.0, 1.0);
        EXPECT_GE(v.x(), 0.0);
        EXPECT_LE(v.x(), 1.0);
        EXPECT_GE(v.y(), 0.0);
        EXPECT_LE(v.y(), 1.0);
        EXPECT_GE(v.z(), 0.0);
        EXPECT_LE(v.z(), 1.0);
    }
}

TEST_F(Vec3Test, RandomInUnitSphere) {
    for (int i = 0; i < 100; i++) {
        Vec3 v = randomInUnitSphere();
        EXPECT_LT(v.lengthSquared(), 1.0);
    }
}

TEST_F(Vec3Test, RandomUnitVectorHasLengthOne) {
    for (int i = 0; i < 100; i++) {
        Vec3 v = randomUnitVector();
        EXPECT_NEAR(v.length(), 1.0, 1e-10);
    }
}

TEST_F(Vec3Test, RandomOnHemisphere) {
    Vec3 normal(0, 1, 0);
    for (int i = 0; i < 100; i++) {
        Vec3 v = randomOnHemisphere(normal);
        EXPECT_GE(dot(v, normal), 0.0);
        EXPECT_NEAR(v.length(), 1.0, 1e-10);
    }
}

TEST_F(Vec3Test, RandomInUnitDisk) {
    for (int i = 0; i < 100; i++) {
        Vec3 v = randomInUnitDisk();
        EXPECT_DOUBLE_EQ(v.z(), 0.0);
        EXPECT_LT(v.lengthSquared(), 1.0);
    }
}
