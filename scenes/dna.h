#pragma once

#include "../src/scene.h"

inline scene generate_scene()
{
    hittable_list world;

    auto mat_backbone = std::make_shared<dielectric>(1.5);
    auto mat_red = std::make_shared<lambertian>(color(0.9, 0.1, 0.1));
    auto mat_blue = std::make_shared<lambertian>(color(0.1, 0.2, 0.9));
    auto mat_floor = std::make_shared<lambertian>(color(0.6, 0.6, 0.6));

    // Floor
    world.add(std::make_shared<sphere>(point3(0, -1005, 0), 1000, mat_floor));

    int num_steps = 140;
    real radius = 4.0f;
    real height_step = 0.5f;
    real rotation_speed = 0.22f;

    for (int i = 0; i < num_steps; i++)
    {
        real phi = i * rotation_speed;
        // Centering the strand along the x axis
        real x = (i - num_steps / 2.0f) * height_step;

        point3 p1(x, radius * std::cos(phi) + 5.0f, radius * std::sin(phi));
        point3 p2(x, radius * std::cos(phi + pi) + 5.0f, radius * std::sin(phi + pi));

        // Rails
        world.add(std::make_shared<sphere>(p1, 0.5f, mat_backbone));
        world.add(std::make_shared<sphere>(p2, 0.5f, mat_backbone));

        // Rungs
        if (i % 4 == 0)
        {
            int rungs_per_connection = 9;
            for (int j = 1; j < rungs_per_connection; j++)
            {
                real t = (real)j / rungs_per_connection;
                point3 rung_pos = (1.0f - t) * p1 + t * p2;

                // Red on one side, blue on the other
                auto rung_mat = (t < 0.5) ? mat_red : mat_blue;
                world.add(std::make_shared<sphere>(rung_pos, 0.25f, rung_mat));
            }
        }
    }

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;

    cam.vfov = 30;
    cam.lookfrom = point3(35, 20, 30);
    cam.lookat = point3(0, 5, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 1.0;
    cam.focus_dist = 45.0;

    return {world, cam};
}