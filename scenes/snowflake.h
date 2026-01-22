#pragma once

#include "../src/scene.h"

void add_sphere_flake(hittable_list &world, point3 center, real radius, int depth, std::shared_ptr<material> mat)
{
    world.add(std::make_shared<sphere>(center, radius, mat));
    if (depth <= 0)
        return;

    real new_radius = radius * 0.45f;
    real offset = radius + new_radius;

    // Recursive branches
    add_sphere_flake(world, center + vec3(offset, 0, 0), new_radius, depth - 1, mat);
    add_sphere_flake(world, center + vec3(-offset, 0, 0), new_radius, depth - 1, mat);
    add_sphere_flake(world, center + vec3(0, offset, 0), new_radius, depth - 1, mat);
    add_sphere_flake(world, center + vec3(0, -offset, 0), new_radius, depth - 1, mat);
    add_sphere_flake(world, center + vec3(0, 0, offset), new_radius, depth - 1, mat);
    add_sphere_flake(world, center + vec3(0, 0, -offset), new_radius, depth - 1, mat);
}

inline scene generate_scene()
{
    hittable_list world;

    // 1. The Fractal "Monument" (Centerpiece)
    auto fractal_mat = std::make_shared<metal>(color(0.9, 0.9, 0.9), 0.05);
    // Depth 5 = ~9,331 spheres. This is a true BVH stress test.
    add_sphere_flake(world, point3(0, 1.5, 0), 1.5f, 5, fractal_mat);

    // 2. The Reflective Floor
    auto floor_mat = std::make_shared<metal>(color(0.5, 0.5, 0.5), 0.1);
    world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, floor_mat));

    // 3. Scattering colored "Book Cover" spheres on the floor
    // Using a loop to place them around the monument
    for (int i = 0; i < 12; i++)
    {
        real angle = i * (2.0f * 3.14159f / 12.0f);
        real x = 5.0f * std::cos(angle);
        real z = 5.0f * std::sin(angle);

        // Vary the materials: some matte, some metal, some glass
        std::shared_ptr<material> sphere_mat;
        if (i % 3 == 0)
            sphere_mat = std::make_shared<dielectric>(1.5);
        else if (i % 3 == 1)
            sphere_mat = std::make_shared<metal>(color(random_real(0.5, 1), random_real(0.5, 1), random_real(0.5, 1)), 0.1);
        else
            sphere_mat = std::make_shared<lambertian>(color(random_real(), random_real(), random_real()));

        world.add(std::make_shared<sphere>(point3(x, 0.3, z), 0.3, sphere_mat));
    }

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;      // High res for detail
    cam.samples_per_pixel = 500; // Clean up the deep fractal shadows
    cam.max_depth = 50;

    cam.vfov = 30;
    cam.lookfrom = point3(13, 5, 13);
    cam.lookat = point3(0, 1.5, 0); // Center on the monument
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.4;
    cam.focus_dist = 18.0;

    return {world, cam};
}