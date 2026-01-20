#pragma once

#include <cmath>
#include <format>
#include <print>

struct vec3
{
    double x{0}, y{0}, z{0};

    constexpr vec3() = default;
    constexpr vec3(double x, double y, double z) : x{x}, y{y}, z{z} {}

    // Negation
    [[nodiscard]] constexpr vec3 operator-() const { return {-x, -y, -z}; }

    // Subscript access
    [[nodiscard]] constexpr double operator[](int i) const { return (&x)[i]; }
    [[nodiscard]] double &operator[](int i) { return (&x)[i]; }

    // Compound assignments
    constexpr vec3 &operator+=(const vec3 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    constexpr vec3 &operator*=(double t)
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    constexpr vec3 &operator/=(double t) { return *this *= (1.0 / t); }

    // Length methods
    [[nodiscard]] constexpr double length() const
    {
        return std::sqrt(length_squared());
    }
    [[nodiscard]] constexpr double length_squared() const
    {
        return x * x + y * y + z * z;
    }
};

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;  // RGB color

// -- vec3 Utility Functions --

[[nodiscard]] constexpr vec3 operator+(const vec3 &u, const vec3 &v)
{
    return {u.x + v.x, u.y + v.y, u.z + v.z};
}

[[nodiscard]] constexpr vec3 operator-(const vec3 &u, const vec3 &v)
{
    return {u.x - v.x, u.y - v.y, u.z - v.z};
}

[[nodiscard]] constexpr vec3 operator*(const vec3 &u, const vec3 &v)
{
    return {u.x * v.x, u.y * v.y, u.z * v.z};
}

[[nodiscard]] constexpr vec3 operator*(double t, const vec3 &v)
{
    return {t * v.x, t * v.y, t * v.z};
}

[[nodiscard]] constexpr vec3 operator*(const vec3 &v, double t)
{
    return t * v;
}

[[nodiscard]] constexpr vec3 operator/(const vec3 &v, double t)
{
    return (1.0 / t) * v;
}

[[nodiscard]] constexpr double dot(const vec3 &u, const vec3 &v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

[[nodiscard]] constexpr vec3 cross(const vec3 &u, const vec3 &v)
{
    return {u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

[[nodiscard]] constexpr vec3 unit_vector(const vec3 &v)
{
    return v / v.length();
}

// --- C++20/23/26 Formatter ---
// This enables std::print and std::format to work with vec3
template <>
struct std::formatter<vec3> : std::formatter<string_view>
{
    auto format(const vec3 &v, format_context &ctx) const
    {
        return format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
    }
};