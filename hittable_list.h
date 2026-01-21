#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list : public hittable
{
public:
    // Container of unique pointers to ensure zero-overhead ownership
    std::vector<std::unique_ptr<hittable>> objects;

    hittable_list() = default;

    // Explicit constructor for a single object
    explicit hittable_list(std::unique_ptr<hittable> object)
    {
        add(std::move(object));
    }

    void clear() { objects.clear(); }

    // Use std::move because unique_ptr cannot be copied
    void add(std::unique_ptr<hittable> object)
    {
        objects.push_back(std::move(object));
    }

    [[nodiscard]] constexpr bool hit(
        const ray &r,
        interval ray_t,
        hit_record &rec) const override
    {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto &object : objects)
        {
            // As we find closer objects, we shrink 'closest_so_far'
            // This ignores any objects further away than the one we just hit
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};