#include <gtest/gtest.h>
#include "../../src/utils/onb.h"

using namespace raytracer;

class ONBFixture : public ::testing::Test {
protected:
    ONB onb;
};

TEST_F(ONBFixture, BuildsOrthonormalBasisFromAxisAlignedNormal) {
    onb.buildFromW(Vec3(0, 0, 1));

    EXPECT_NEAR(dot(onb.u(), onb.v()), 0.0, 1e-12);
    EXPECT_NEAR(dot(onb.u(), onb.w()), 0.0, 1e-12);
    EXPECT_NEAR(dot(onb.v(), onb.w()), 0.0, 1e-12);

    EXPECT_NEAR(onb.u().length(), 1.0, 1e-12);
    EXPECT_NEAR(onb.v().length(), 1.0, 1e-12);
    EXPECT_NEAR(onb.w().length(), 1.0, 1e-12);

    EXPECT_DOUBLE_EQ(onb.w().x(), 0.0);
    EXPECT_DOUBLE_EQ(onb.w().y(), 0.0);
    EXPECT_DOUBLE_EQ(onb.w().z(), 1.0);

    Vec3 world = onb.local(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(world.x(), -1.0);
    EXPECT_DOUBLE_EQ(world.y(), 2.0);
    EXPECT_DOUBLE_EQ(world.z(), 3.0);
}

TEST_F(ONBFixture, LocalTransformUsesComputedBasis) {
    onb.buildFromW(Vec3(1.0, 1.0, 1.0));

    Vec3 expected = onb.u() + 2.0 * onb.v() + 3.0 * onb.w();
    Vec3 transformed = onb.local(1.0, 2.0, 3.0);

    EXPECT_NEAR(transformed.x(), expected.x(), 1e-12);
    EXPECT_NEAR(transformed.y(), expected.y(), 1e-12);
    EXPECT_NEAR(transformed.z(), expected.z(), 1e-12);
}
