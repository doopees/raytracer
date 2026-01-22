#pragma once

#include "common.h"
#include "hittable.h"
#include "aabb.h"

class sphere : public hittable
{
public:
    constexpr sphere(const point3 &center, real radius, std::shared_ptr<material> mat)
        : center(center), radius(std::max(0.0f, radius)), mat(mat)
    {
        // The bounding box goes from (center - radius) to (center + radius)
        auto r_vec = vec3(radius, radius, radius);
        bbox = aabb(center - r_vec, center + r_vec);
    }

    [[nodiscard]] bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        vec3 oc = center - r.origin();
        // Simplified quadratic: a*t^2 + 2ht + c = 0
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0.0f)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range (tmin, tmax)
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

    [[nodiscard]] aabb bounding_box() const override { return bbox; }

private:
    point3 center;
    real radius;
    std::shared_ptr<material> mat;
    aabb bbox;
};
