#include "common.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include <chrono>

int main()
{
    // Camera
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    // World
    hittable_list world;

    auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<dielectric>(1.50);
    auto material_bubble = std::make_shared<dielectric>(1.00 / 1.50);
    auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(std::make_unique<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground)); // Ground
    world.add(std::make_unique<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));      // Center sphere
    world.add(std::make_unique<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));       // Left sphere
    world.add(std::make_unique<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));     // Bubble inside left sphere
    world.add(std::make_unique<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));       // Right sphere

    // Rendering
    auto start_time = std::chrono::high_resolution_clock::now();
    cam.render(world);
    auto end_time = std::chrono::high_resolution_clock::now();

    // Report render time
    std::chrono::duration<float, std::milli> duration = end_time - start_time;
    std::println(stderr, "Total render time: {:.2f}ms", duration.count());

    // Convert PPM to PNG using ffmpeg (if available)
    std::system("ffmpeg -y -i output.ppm output.png > nul 2>&1");

    return 0;
}