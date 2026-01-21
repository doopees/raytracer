#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <numbers>
#include <random>

// Constants
inline constexpr double infinity = std::numeric_limits<double>::infinity();
inline constexpr double pi = std::numbers::pi;

// Utility Functions
[[nodiscard]] constexpr double degrees_to_radians(double degrees) noexcept
{
    return degrees * pi / 180.0;
}

// Returns a random real in [0,1)
[[nodiscard]] inline double random_double()
{
    // static ensures we only initialize the generator once, not every call
    static std::mt19937 generator{std::random_device{}()};
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

// Overload for a specific range [min, max)
[[nodiscard]] inline double random_double(double min, double max)
{
    return min + (max - min) * random_double();
}

// Common Headers (Optional: include these if you want a "master" header)
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"