#pragma once
#include "../src/scene.h"

inline scene generate_scene()
{
    hittable_list world;

    // Floor: Subtle grid/matte
    auto ground_mat = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(point3(0, -100.5, 0), 100, ground_mat));

    // Material Row
    world.add(std::make_shared<sphere>(point3(-2.2, 0, -1), 0.5, std::make_shared<lambertian>(color(0.8, 0.2, 0.2)))); // Matte Red
    world.add(std::make_shared<sphere>(point3(-1.1, 0, -1), 0.5, std::make_shared<metal>(color(0.8, 0.8, 0.8), 0.0))); // Silver
    world.add(std::make_shared<sphere>(point3(0.0, 0, -1), 0.5, std::make_shared<dielectric>(1.5)));                   // Glass
    world.add(std::make_shared<sphere>(point3(1.1, 0, -1), 0.5, std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.2)));  // Gold (Rough)
    world.add(std::make_shared<sphere>(point3(2.2, 0, -1), 0.5, std::make_shared<dielectric>(1.5)));                   // Bubble
    // To make a bubble, add a sphere with negative radius inside a glass sphere (if your engine supports it)
    // or just a second glass sphere with ir=1/1.5.

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 500;
    cam.lookfrom = point3(0, 1, 0);
    cam.lookat = point3(0, 0, -1);

    return {world, cam};
}