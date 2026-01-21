#pragma once

#include "interval.h"
#include "vec3.h"
#include <cstdint>

using color = vec3;

struct Pixel
{
    std::uint8_t r, g, b;
};

// Convert a color to a Pixel by clamping and scaling
[[nodiscard]] constexpr Pixel to_pixel(const color &pixel_color) noexcept
{
    // The book uses [0, 0.999] to avoid rounding up to 256
    static constexpr interval intensity(0.000, 0.999);

    // Multiplying [0, 0.999] by 256 gives a max of 255.744,
    // which correctly truncates to the integer 255.
    return Pixel{
        .r = static_cast<std::uint8_t>(256 * intensity.clamp(pixel_color.x)),
        .g = static_cast<std::uint8_t>(256 * intensity.clamp(pixel_color.y)),
        .b = static_cast<std::uint8_t>(256 * intensity.clamp(pixel_color.z))};
}