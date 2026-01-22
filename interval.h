#pragma once

#include "common.h"

class interval
{
public:
    real min, max;

    // Constructors
    constexpr interval() : min(+infinity), max(-infinity) {}
    constexpr interval(real min, real max) : min(min), max(max) {}
    constexpr interval(const interval &a, const interval &b)
        : min(std::min(a.min, b.min)), max(std::max(a.max, b.max)) {}

    [[nodiscard]] constexpr real size() const noexcept
    {
        return max - min;
    }

    [[nodiscard]] constexpr bool contains(real x) const noexcept
    {
        return min <= x && x <= max;
    }

    [[nodiscard]] constexpr bool surrounds(real x) const noexcept
    {
        return min < x && x < max;
    }

    // Clamp utility: stays within [min, max]
    [[nodiscard]] constexpr real clamp(real x) const noexcept
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