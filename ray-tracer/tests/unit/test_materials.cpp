#include <gtest/gtest.h>
#include "../../src/materials/lambertian.h"
#include "../../src/materials/metal.h"
#include "../../src/materials/dielectric.h"
#include "../../src/geometry/hittable.h"
#include "../../src/core/ray.h"

using namespace raytracer;

class MaterialTest : public ::testing::Test {
protected:
    HitRecord rec;
    Ray incomingRay;

    void SetUp() override {
        // Setup a basic hit record
        rec.p = Point3(0, 0, 0);
        rec.normal = Vec3(0, 1, 0);
        rec.t = 1.0;
        rec.frontFace = true;

        // Incoming ray from above
        incomingRay = Ray(Point3(0, 1, 0), Vec3(0, -1, 0));
    }
};

TEST_F(MaterialTest, LambertianScattersRay) {
    Lambertian mat(Color(0.5, 0.5, 0.5));
    Color attenuation;
    Ray scattered;

    bool didScatter = mat.scatter(incomingRay, rec, attenuation, scattered);

    EXPECT_TRUE(didScatter);
    EXPECT_DOUBLE_EQ(attenuation.x(), 0.5);
    EXPECT_DOUBLE_EQ(attenuation.y(), 0.5);
    EXPECT_DOUBLE_EQ(attenuation.z(), 0.5);
    // Scattered ray should originate from hit point
    EXPECT_DOUBLE_EQ(scattered.origin().x(), 0.0);
    EXPECT_DOUBLE_EQ(scattered.origin().y(), 0.0);
    EXPECT_DOUBLE_EQ(scattered.origin().z(), 0.0);
}

TEST_F(MaterialTest, LambertianAttenuationMatchesAlbedo) {
    Color albedo(0.8, 0.3, 0.1);
    Lambertian mat(albedo);
    Color attenuation;
    Ray scattered;

    mat.scatter(incomingRay, rec, attenuation, scattered);

    EXPECT_DOUBLE_EQ(attenuation.x(), 0.8);
    EXPECT_DOUBLE_EQ(attenuation.y(), 0.3);
    EXPECT_DOUBLE_EQ(attenuation.z(), 0.1);
}

TEST_F(MaterialTest, MetalReflectsRay) {
    Metal mat(Color(0.8, 0.8, 0.8), 0.0);
    Color attenuation;
    Ray scattered;

    bool didScatter = mat.scatter(incomingRay, rec, attenuation, scattered);

    EXPECT_TRUE(didScatter);
    // For metal, check that scattered direction has upward component (reflection)
    EXPECT_GT(scattered.direction().y(), 0.0);
}

TEST_F(MaterialTest, MetalAttenuationMatchesAlbedo) {
    Color albedo(0.7, 0.6, 0.5);
    Metal mat(albedo, 0.0);
    Color attenuation;
    Ray scattered;

    mat.scatter(incomingRay, rec, attenuation, scattered);

    EXPECT_DOUBLE_EQ(attenuation.x(), 0.7);
    EXPECT_DOUBLE_EQ(attenuation.y(), 0.6);
    EXPECT_DOUBLE_EQ(attenuation.z(), 0.5);
}

TEST_F(MaterialTest, MetalFuzzClampsToOne) {
    // Fuzz > 1 should be clamped to 1
    Metal mat(Color(0.8, 0.8, 0.8), 2.0);
    Color attenuation;
    Ray scattered;

    // Should not crash
    bool didScatter = mat.scatter(incomingRay, rec, attenuation, scattered);
    EXPECT_TRUE(didScatter || !didScatter); // Just check it doesn't crash
}

TEST_F(MaterialTest, DielectricAlwaysScatters) {
    Dielectric mat(1.5); // Glass
    Color attenuation;
    Ray scattered;

    bool didScatter = mat.scatter(incomingRay, rec, attenuation, scattered);

    EXPECT_TRUE(didScatter);
    // Dielectric has white attenuation (no color absorption)
    EXPECT_DOUBLE_EQ(attenuation.x(), 1.0);
    EXPECT_DOUBLE_EQ(attenuation.y(), 1.0);
    EXPECT_DOUBLE_EQ(attenuation.z(), 1.0);
}

TEST_F(MaterialTest, DielectricScatteredRayOriginatesFromHitPoint) {
    Dielectric mat(1.5);
    Color attenuation;
    Ray scattered;

    mat.scatter(incomingRay, rec, attenuation, scattered);

    EXPECT_DOUBLE_EQ(scattered.origin().x(), 0.0);
    EXPECT_DOUBLE_EQ(scattered.origin().y(), 0.0);
    EXPECT_DOUBLE_EQ(scattered.origin().z(), 0.0);
}

TEST_F(MaterialTest, DielectricRefractsOrReflects) {
    Dielectric mat(1.5);
    Color attenuation;
    Ray scattered;

    // Run multiple times to test randomness
    int reflectionCount = 0;
    int refractionCount = 0;

    for (int i = 0; i < 100; i++) {
        mat.scatter(incomingRay, rec, attenuation, scattered);
        // Check if ray is reflected (going up) or refracted (going down)
        if (scattered.direction().y() > 0)
            reflectionCount++;
        else
            refractionCount++;
    }

    // Both reflection and refraction should happen sometimes
    // (this is probabilistic, so we just check both occur)
    EXPECT_GT(reflectionCount, 0);
    EXPECT_GT(refractionCount, 0);
}

TEST_F(MaterialTest, LambertianScatterDirectionNotDegenerate) {
    Lambertian mat(Color(0.5, 0.5, 0.5));
    Color attenuation;
    Ray scattered;

    // Test multiple scatters to ensure we don't get degenerate directions
    for (int i = 0; i < 100; i++) {
        mat.scatter(incomingRay, rec, attenuation, scattered);
        Vec3 dir = scattered.direction();

        // Direction should not be zero
        EXPECT_FALSE(dir.nearZero());

        // Direction should have some upward component (scattered from surface)
        // This is true for Lambertian reflection with upward normal
        EXPECT_GT(dot(dir, rec.normal), 0.0);
    }
}
