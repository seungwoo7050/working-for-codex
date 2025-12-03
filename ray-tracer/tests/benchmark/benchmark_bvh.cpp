#include <iostream>
#include <chrono>
#include "geometry/bvh_node.h"
#include "geometry/hittable_list.h"
#include "geometry/sphere.h"
#include "materials/lambertian.h"
#include "core/color.h"
#include "core/ray.h"

using namespace raytracer;
using namespace std::chrono;

/**
 * Performance benchmark comparing BVH vs naive linear search
 */

HittableList createRandomScene(int numSpheres) {
    HittableList world;
    auto material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));

    for (int i = 0; i < numSpheres; i++) {
        auto center = Point3(
            randomDouble(-10, 10),
            randomDouble(-10, 10),
            randomDouble(-20, -5)
        );
        world.add(make_shared<Sphere>(center, 0.5, material));
    }

    return world;
}

void benchmarkHitTests(const Hittable& world, const std::string& name, int numRays) {
    auto start = high_resolution_clock::now();

    int hits = 0;
    for (int i = 0; i < numRays; i++) {
        Ray r(
            Point3(0, 0, 0),
            Vec3(randomDouble(-1, 1), randomDouble(-1, 1), -1)
        );
        HitRecord rec;
        if (world.hit(r, 0.001, 100.0, rec)) {
            hits++;
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    std::cout << name << ":\n";
    std::cout << "  Rays: " << numRays << "\n";
    std::cout << "  Hits: " << hits << "\n";
    std::cout << "  Time: " << duration.count() << " ms\n";
    std::cout << "  Rays/sec: " << (numRays * 1000.0 / duration.count()) << "\n\n";
}

int main() {
    std::cout << "=== BVH Performance Benchmark ===\n\n";

    const int numSpheres = 1000;
    const int numRays = 10000;

    std::cout << "Creating scene with " << numSpheres << " spheres...\n\n";

    // Test 1: Naive linear search
    auto scene1 = createRandomScene(numSpheres);
    benchmarkHitTests(scene1, "Naive (HittableList)", numRays);

    // Test 2: BVH acceleration
    auto scene2 = createRandomScene(numSpheres);
    BVHNode bvh(scene2, 0, 1);
    benchmarkHitTests(bvh, "BVH Accelerated", numRays);

    return 0;
}
