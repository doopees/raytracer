#pragma once

#include "hittable.h"

class material
{
public:
    virtual ~material() = default;

    // Returns true if the ray was scattered, and provides the
    // resulting attenuation (color) and the new scattered ray.
    [[nodiscard]] virtual bool scatter(
        const ray &r_in,
        const hit_record &rec,
        color &attenuation,
        ray &scattered) const = 0;
};

class lambertian : public material
{
public:
    lambertian(const color &albedo) : albedo(albedo) {}

    bool scatter([[maybe_unused]] const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction (if normal + random is zero)
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

class metal : public material
{
public:
    metal(const color &albedo, real fuzz) : albedo(albedo), fuzz(fuzz < 1.0f ? fuzz : 1.0f) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0.0f);
    }

private:
    color albedo;
    real fuzz; // Fuzziness factor (0 = perfect mirror, 1 = very fuzzy)
};

class dielectric : public material
{
public:
    dielectric(real refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        attenuation = color(1.0f, 1.0f, 1.0f); // Glass doesn't absorb light
        real refraction_ratio = rec.front_face ? (1.0f / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        real cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0f);
        real sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);
        bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_real())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);
        return true;
    }

private:
    // Refraction index of the material or ratio of the material's
    // refraction index over surrounding refraction index
    real refraction_index;

    [[nodiscard]] static constexpr real reflectance(real cosine, real refraction_index) noexcept
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
        r0 = r0 * r0;
        // std::pow is heavy; for an integer power like 5, simple multiplication is faster
        real x = 1.0f - cosine;
        return r0 + (1.0f - r0) * (x * x * x * x * x);
    }
};