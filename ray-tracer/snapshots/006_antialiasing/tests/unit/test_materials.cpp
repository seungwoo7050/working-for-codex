#include <gtest/gtest.h>
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/dielectric.h"
#include "geometry/hittable.h"

using namespace raytracer;

TEST(LambertianTest, AlwaysScatters) {
    Lambertian mat(Color(0.5, 0.5, 0.5));
    
    HitRecord rec;
    rec.p = Point3(0, 0, 0);
    rec.normal = Vec3(0, 1, 0);
    rec.frontFace = true;
    
    Ray rIn(Point3(0, 1, 0), Vec3(0, -1, 0));
    Color attenuation;
    Ray scattered;
    
    EXPECT_TRUE(mat.scatter(rIn, rec, attenuation, scattered));
    EXPECT_DOUBLE_EQ(attenuation.x(), 0.5);
}

TEST(MetalTest, ReflectsRay) {
    Metal mat(Color(0.8, 0.8, 0.8), 0.0);
    
    HitRecord rec;
    rec.p = Point3(0, 0, 0);
    rec.normal = Vec3(0, 1, 0);
    rec.frontFace = true;
    
    Ray rIn(Point3(-1, 1, 0), unitVector(Vec3(1, -1, 0)));
    Color attenuation;
    Ray scattered;
    
    EXPECT_TRUE(mat.scatter(rIn, rec, attenuation, scattered));
}

TEST(DielectricTest, RefractsOrReflects) {
    Dielectric mat(1.5);
    
    HitRecord rec;
    rec.p = Point3(0, 0, 0);
    rec.normal = Vec3(0, 1, 0);
    rec.frontFace = true;
    
    Ray rIn(Point3(0, 1, 0), Vec3(0, -1, 0));
    Color attenuation;
    Ray scattered;
    
    EXPECT_TRUE(mat.scatter(rIn, rec, attenuation, scattered));
    // Attenuation should be white for dielectric
    EXPECT_DOUBLE_EQ(attenuation.x(), 1.0);
}
