#include "common.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

#include <chrono>

int main()
{
    // Camera
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;

    // World
    hittable_list world;
    world.add(std::make_unique<sphere>(point3(0, -100.5, -1), 100)); // Ground
    world.add(std::make_unique<sphere>(point3(0, 0, -1), 0.5));      // Object

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