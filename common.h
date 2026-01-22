#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <numbers>
#include <random>

using real = float;

// Constants
inline constexpr real infinity = std::numeric_limits<real>::infinity();
inline constexpr real pi = std::numbers::pi;

// Utility Functions
[[nodiscard]] constexpr real degrees_to_radians(real degrees) noexcept
{
    return degrees * pi / 180.0f;
}

// Returns a random real in [0,1)
[[nodiscard]] inline real random_real()
{
    // thread_local ensures each thread has its own unique generator and distribution
    static thread_local std::mt19937 generator{std::random_device{}()};
    static thread_local std::uniform_real_distribution<real> distribution(0.0f, 1.0f);
    return distribution(generator);
}

// Overload for a specific range [min, max)
[[nodiscard]] inline real random_real(real min, real max)
{
    return min + (max - min) * random_real();
}

// Common Headers (Optional: include these if you want a "master" header)
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"