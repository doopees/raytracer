#pragma once

#include "hittable_list.h"
#include "camera.h"

#include "material.h"
#include "sphere.h"

struct scene
{
    hittable_list world;
    camera cam;

    scene(hittable_list world, const camera cam)
        : world(std::move(world)), cam(cam) {}
};