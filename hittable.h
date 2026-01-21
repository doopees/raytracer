#pragma once

#include "ray.h"

struct hit_record
{
    point3 p;
    vec3 normal;
    double t;
    bool front_face;

    // Sets the hit record normal vector.
    // NOTE: the parameter `outward_normal` is assumed to have unit length.
    constexpr void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;

    [[nodiscard]] virtual bool hit(
        const ray &r, interval ray_t, hit_record &rec) const = 0;
};
