#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <numbers>

// Constants
inline constexpr double infinity = std::numeric_limits<double>::infinity();
inline constexpr double pi = std::numbers::pi;

// Utility Functions
[[nodiscard]] constexpr double degrees_to_radians(double degrees) noexcept
{
    return degrees * pi / 180.0;
}

// Common Headers (Optional: include these if you want a "master" header)
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"