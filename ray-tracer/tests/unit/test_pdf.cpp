#include <gtest/gtest.h>
#include "../../src/utils/pdf.h"
#include "../../src/utils/onb.h"
#include "../../src/geometry/sphere.h"
#include "../../src/geometry/hittable_list.h"
#include "../../src/materials/lambertian.h"

using namespace raytracer;

// Test ONB construction
TEST(ONBTest, BuildFromW) {
    Vec3 normal(0, 1, 0);
    ONB onb;
    onb.buildFromW(normal);

    // Check that w is aligned with normal
    EXPECT_NEAR(onb.w().y(), 1.0, 1e-6);

    // Check orthogonality
    EXPECT_NEAR(dot(onb.u(), onb.v()), 0.0, 1e-6);
    EXPECT_NEAR(dot(onb.u(), onb.w()), 0.0, 1e-6);
    EXPECT_NEAR(dot(onb.v(), onb.w()), 0.0, 1e-6);

    // Check that all vectors are unit length
    EXPECT_NEAR(onb.u().length(), 1.0, 1e-6);
    EXPECT_NEAR(onb.v().length(), 1.0, 1e-6);
    EXPECT_NEAR(onb.w().length(), 1.0, 1e-6);
}

// Test CosinePDF normalization
TEST(PDFTest, CosinePDFNormalization) {
    Vec3 normal(0, 0, 1);
    CosinePDF pdf(normal);

    // Test that PDF value is reasonable
    Vec3 direction(0, 0, 1); // Direction along normal
    double value = pdf.value(direction);

    // For direction along normal, PDF should be 1/pi
    EXPECT_NEAR(value, 1.0 / PI, 1e-6);

    // For perpendicular direction, PDF should be 0
    Vec3 perp(1, 0, 0);
    double perpValue = pdf.value(perp);
    EXPECT_NEAR(perpValue, 0.0, 1e-6);
}

// Test SpherePDF
TEST(PDFTest, SpherePDFUniform) {
    SpherePDF pdf;

    // Sphere PDF should be uniform (1 / 4π)
    Vec3 direction(0, 0, 1);
    double value = pdf.value(direction);
    EXPECT_NEAR(value, 1.0 / (4.0 * PI), 1e-6);
}

// Test HittablePDF with sphere
TEST(PDFTest, HittablePDFSphere) {
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    Sphere sphere(Point3(0, 0, 0), 1.0, material);

    Point3 origin(0, 0, 5);
    HittablePDF pdf(sphere, origin);

    // Generate a direction toward the sphere
    Vec3 direction = pdf.generate();

    // Direction should point generally toward the sphere
    EXPECT_LT(direction.z(), 0.0);
}

// Test MixturePDF
TEST(PDFTest, MixturePDFValue) {
    auto pdf1 = make_shared<SpherePDF>();
    auto pdf2 = make_shared<CosinePDF>(Vec3(0, 0, 1));

    MixturePDF mixedPdf(pdf1, pdf2);

    Vec3 direction(0, 0, 1);
    double value = mixedPdf.value(direction);

    // Should be average of both PDFs
    double expected = 0.5 * pdf1->value(direction) + 0.5 * pdf2->value(direction);
    EXPECT_NEAR(value, expected, 1e-6);
}

// Test importance sampling convergence
TEST(PDFTest, ImportanceSamplingConvergence) {
    Vec3 normal(0, 0, 1);
    CosinePDF pdf(normal);

    int numSamples = 10000;
    double sum = 0.0;

    // Monte Carlo integration test
    for (int i = 0; i < numSamples; i++) {
        Vec3 direction = pdf.generate();

        // Check that generated directions are in the correct hemisphere
        EXPECT_GE(dot(direction, normal), 0.0);

        sum += dot(direction, normal) / pdf.value(direction);
    }

    double average = sum / numSamples;

    // The integral of cos(theta) over hemisphere should be approximately π
    EXPECT_NEAR(average, PI, 0.1);
}

// Test PDF value is always non-negative
TEST(PDFTest, PDFValueNonNegative) {
    Vec3 normal(0, 1, 0);
    CosinePDF pdf(normal);

    // Test multiple random directions
    for (int i = 0; i < 100; i++) {
        Vec3 direction = randomUnitVector();
        double value = pdf.value(direction);
        EXPECT_GE(value, 0.0);
    }
}
