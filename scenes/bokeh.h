#pragma once

#include "../src/scene.h"

inline scene generate_scene()
{
    hittable_list world;

    // Lots of random spheres
    for (int i = 0; i < 800; i++)
    {
        auto center = point3(random_real(-18, 18), random_real(-12, 12), random_real(-25, 8));
        auto radius = random_real(0.15, 1.2);

        auto choose_mat = random_real();
        std::shared_ptr<material> sphere_mat;

        if (choose_mat < 0.6)
        {
            auto albedo = color::random() * color::random();
            sphere_mat = std::make_shared<lambertian>(albedo);
        }
        else if (choose_mat < 0.85)
        {
            auto albedo = color::random(0.5, 1.0);
            auto fuzz = random_real(0, 0.1);
            sphere_mat = std::make_shared<metal>(albedo, fuzz);
        }
        else
        {
            sphere_mat = std::make_shared<dielectric>(1.5);
        }

        world.add(std::make_shared<sphere>(center, radius, std::move(sphere_mat)));
    }

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;

    cam.vfov = 55;
    cam.lookfrom = point3(0, 0, 15);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 1.8;
    cam.focus_dist = 15.0;

    return {world, cam};
}