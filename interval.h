#pragma once

#include "common.h"

class interval
{
public:
    double min, max;

    // Default interval is empty
    constexpr interval() : min(+infinity), max(-infinity) {}

    constexpr interval(double min, double max) : min(min), max(max) {}

    [[nodiscard]] constexpr double size() const noexcept
    {
        return max - min;
    }

    [[nodiscard]] constexpr bool contains(double x) const noexcept
    {
        return min <= x && x <= max;
    }

    [[nodiscard]] constexpr bool surrounds(double x) const noexcept
    {
        return min < x && x < max;
    }

    // Clamp utility: stays within [min, max]
    [[nodiscard]] constexpr double clamp(double x) const noexcept
    {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

    static const interval empty, universe;
};

inline constexpr interval interval::empty = interval(+infinity, -infinity);
inline constexpr interval interval::universe = interval(-infinity, +infinity);