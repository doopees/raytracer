#pragma once

#include "vec3.h"

using real = float;

class ray
{
public:
    constexpr ray() = default;
    constexpr ray(const point3 &origin, const vec3 &direction)
        : orig{origin}, dir{direction} {}

    // Accessors
    [[nodiscard]] constexpr point3 origin() const { return orig; }
    [[nodiscard]] constexpr vec3 direction() const { return dir; }

    // Compute point along ray at parameter t
    [[nodiscard]] constexpr point3 at(real t) const { return orig + t * dir; }

private:
    point3 orig;
    vec3 dir;
};