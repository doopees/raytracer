#pragma once

#include "../src/scene.h"

inline scene generate_scene()
{
    hittable_list world;

    int grid_size = 40;
    real spacing = 1.1f;

    for (int i = 0; i < grid_size; i++)
    {
        for (int j = 0; j < grid_size; j++)
        {
            // Calculate x and z coordinates
            real x = (i - grid_size / 2.0f) * spacing;
            real z = (j - grid_size / 2.0f) * spacing;

            // Calculate height using sine of distance
            real dist = std::sqrt(x * x + z * z);
            real y = std::sin(dist * 0.5f) * 2.5f;

            point3 center(x, y, z);

            // Color based on height
            real color_weight = (y + 2.5f) / 5.0f;
            color sphere_color = (1.0 - color_weight) * color(0.1, 0.2, 0.8) + color_weight * color(0.1, 0.9, 0.9);

            std::shared_ptr<material> sphere_mat;

            // Mix in some glass and metal for variety
            real mat_roll = random_real();
            if (mat_roll < 0.1)
            {
                sphere_mat = std::make_shared<dielectric>(1.5);
            }
            else if (mat_roll < 0.2)
            {
                sphere_mat = std::make_shared<metal>(sphere_color, 0.05);
            }
            else
            {
                sphere_mat = std::make_shared<lambertian>(sphere_color);
            }

            world.add(std::make_shared<sphere>(center, 0.5f, std::move(sphere_mat)));
        }
    }

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;

    cam.vfov = 40;
    cam.lookfrom = point3(20, 18, 20);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.8;
    cam.focus_dist = std::sqrt(20 * 20 + 18 * 18 + 20 * 20);

    return {world, cam};
}