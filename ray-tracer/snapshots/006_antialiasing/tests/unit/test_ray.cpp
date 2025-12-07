#include <gtest/gtest.h>
#include "core/ray.h"

using namespace raytracer;

TEST(RayTest, AtFunction) {
    Point3 origin(0.0, 0.0, 0.0);
    Vec3 direction(1.0, 2.0, 3.0);
    Ray r(origin, direction);
    
    Point3 p = r.at(2.0);
    EXPECT_DOUBLE_EQ(p.x(), 2.0);
    EXPECT_DOUBLE_EQ(p.y(), 4.0);
    EXPECT_DOUBLE_EQ(p.z(), 6.0);
}
