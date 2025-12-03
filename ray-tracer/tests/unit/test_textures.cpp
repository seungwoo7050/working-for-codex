#include <gtest/gtest.h>
#include "textures/solid_color.h"
#include "textures/checker_texture.h"
#include "textures/noise_texture.h"
#include "core/vec3.h"
#include "core/color.h"

using namespace raytracer;

TEST(TextureTest, SolidColorValue) {
    SolidColor texture(Color(0.5, 0.3, 0.2));
    Color c = texture.value(0, 0, Point3(0, 0, 0));

    EXPECT_DOUBLE_EQ(c.x(), 0.5);
    EXPECT_DOUBLE_EQ(c.y(), 0.3);
    EXPECT_DOUBLE_EQ(c.z(), 0.2);
}

TEST(TextureTest, CheckerTextureEven) {
    CheckerTexture texture(1.0, Color(1, 1, 1), Color(0, 0, 0));

    // Test even position (0, 0, 0) -> sum=0, even
    Color c1 = texture.value(0, 0, Point3(0, 0, 0));
    EXPECT_DOUBLE_EQ(c1.x(), 1.0);

    // Test odd position (1, 0, 0) -> sum=1, odd
    Color c2 = texture.value(0, 0, Point3(1, 0, 0));
    EXPECT_DOUBLE_EQ(c2.x(), 0.0);
}

TEST(TextureTest, NoiseTextureCreation) {
    NoiseTexture texture(1.0);
    // Just verify it doesn't crash
    Color c = texture.value(0, 0, Point3(1, 2, 3));

    // Noise should produce values in a reasonable range
    EXPECT_GE(c.x(), -1.0);
    EXPECT_LE(c.x(), 2.0);
}
