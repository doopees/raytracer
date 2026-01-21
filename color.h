#pragma once

#include "vec3.h"
#include <cstdint>

using color = vec3;

struct Pixel
{
    std::uint8_t r, g, b;
};

// Convert a color to a Pixel by clamping and scaling
[[nodiscard]] constexpr Pixel to_pixel(const color &pixel_color)
{
    return {
        static_cast<std::uint8_t>(255.999 * std::clamp(pixel_color.x, 0.0, 0.999)),
        static_cast<std::uint8_t>(255.999 * std::clamp(pixel_color.y, 0.0, 0.999)),
        static_cast<std::uint8_t>(255.999 * std::clamp(pixel_color.z, 0.0, 0.999))};
}
