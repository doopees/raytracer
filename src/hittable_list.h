#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list : public hittable
{
public:
    std::vector<std::shared_ptr<hittable>> objects;

    hittable_list() = default;

    // Explicit constructor for a single object
    explicit hittable_list(std::shared_ptr<hittable> object)
    {
        add(object);
    }

    void clear()
    {
        objects.clear();
        bbox = aabb::empty;
    }

    void add(std::shared_ptr<hittable> object)
    {
        // Update the bounding box to include the new object
        bbox = aabb(bbox, object->bounding_box());
        objects.push_back(object);
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

    [[nodiscard]] aabb bounding_box() const override { return bbox; }

private:
    aabb bbox;
};