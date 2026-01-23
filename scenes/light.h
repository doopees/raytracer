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

    // 1. The Monument: High-polish Chrome
    auto fractal_mat = std::make_shared<metal>(color(0.9, 0.9, 0.9), 0.0);
    // Depth 5 for massive detail
    add_sphere_flake(world, point3(0, 2.5, 0), 2.5f, 5, fractal_mat);

    // 2. The Lights: Emissive "Power Cores"
    // We'll place them strategically inside/around the fractal to catch reflections
    auto teal_light = std::make_shared<diffuse_light>(color(0.0, 8.0, 8.0));
    auto orange_light = std::make_shared<diffuse_light>(color(10.0, 4.0, 1.0));

    // Floating orbs flanking the monument
    world.add(std::make_shared<sphere>(point3(-4, 3, 0), 1.0, teal_light));
    world.add(std::make_shared<sphere>(point3(4, 3, 0), 1.0, orange_light));

    // 3. The Floor: Dark, slightly rough metal to catch light "pools"
    auto floor_mat = std::make_shared<metal>(color(0.1, 0.1, 0.1), 0.15);
    world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, floor_mat));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 800; // Emitters create noise, needs samples!
    cam.max_depth = 50;

    // CAMERA POSITION: Closer and lower for "Hero" scale
    cam.vfov = 40;
    cam.lookfrom = point3(8, 4, 10);
    cam.lookat = point3(0, 2.5, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.2;
    cam.focus_dist = 13.0;

    return {world, cam};
}