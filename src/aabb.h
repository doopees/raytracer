#pragma once
#include "common.h"

class aabb
{
public:
    interval x, y, z;

    aabb() {} // Default empty
    aabb(const interval &x, const interval &y, const interval &z)
        : x(x), y(y), z(z) {}

    // Build a box from two points
    aabb(const point3 &a, const point3 &b)
    {
        x = interval(std::min(a.x, b.x), std::max(a.x, b.x));
        y = interval(std::min(a.y, b.y), std::max(a.y, b.y));
        z = interval(std::min(a.z, b.z), std::max(a.z, b.z));
    }

    // Build a box from two boxes
    aabb(const aabb &box0, const aabb &box1)
    {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }

    const interval &axis(int n) const
    {
        if (n == 1)
            return y;
        if (n == 2)
            return z;
        return x;
    }

    // Ray-box intersection test
    [[nodiscard]] constexpr bool hit(const ray &r, interval ray_t) const
    {
        const point3 &ray_orig = r.origin();
        const vec3 &ray_dir = r.direction();

        for (int axis = 0; axis < 3; axis++)
        {
            const interval &ax = this->axis(axis);
            const real adinv = 1.0f / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1)
            {
                if (t0 > ray_t.min)
                    ray_t.min = t0;
                if (t1 < ray_t.max)
                    ray_t.max = t1;
            }
            else
            {
                if (t1 > ray_t.min)
                    ray_t.min = t1;
                if (t0 < ray_t.max)
                    ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

    // Returns the index of the longest side (0:x, 1:y, 2:z)
    int longest_axis() const
    {
        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    static const aabb empty, universe;
};

// Define the static constants
inline const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
inline const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);