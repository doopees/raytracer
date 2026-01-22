#pragma once

#include "../src/scene.h"

inline scene generate_scene()
{
    hittable_list world;

    auto mat_white = std::make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto mat_red = std::make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto mat_green = std::make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto mat_glass = std::make_shared<dielectric>(1.5);
    auto mat_metal = std::make_shared<metal>(color(0.8, 0.8, 0.8), 0.1);

    // Left wall
    world.add(std::make_shared<sphere>(point3(-1010, 0, 0), 1000, mat_red));
    // Right wall
    world.add(std::make_shared<sphere>(point3(1010, 0, 0), 1000, mat_green));
    // Floor
    world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, mat_white));
    // Back wall
    world.add(std::make_shared<sphere>(point3(0, 0, -1015), 1000, mat_white));

    // Objects
    world.add(std::make_shared<sphere>(point3(-3, 2, -5), 2.0, mat_white));
    world.add(std::make_shared<sphere>(point3(3, 2, -2), 2.0, mat_glass));
    world.add(std::make_shared<sphere>(point3(0, 1, 2), 1.0, mat_metal));

    camera cam;
    cam.aspect_ratio = 1.0;
    cam.image_width = 800;
    cam.samples_per_pixel = 1000; // High samples needed due to low light
    cam.max_depth = 50;

    cam.vfov = 40;
    cam.lookfrom = point3(0, 5, 20);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    return {world, cam};
}