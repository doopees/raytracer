#pragma once

using real = float;

struct alignas(16) vec3
{
    real x{0}, y{0}, z{0};
    real _pad{0}; // To make the struct 16 bytes exactly

    constexpr vec3() = default;
    constexpr vec3(real x, real y, real z) : x{x}, y{y}, z{z} {}

    [[nodiscard]] constexpr vec3 operator-() const noexcept { return {-x, -y, -z}; }

    [[nodiscard]] constexpr real operator[](int i) const noexcept
    {
        return (i == 0) ? x : (i == 1) ? y
                                       : z;
    }

    [[nodiscard]] constexpr real &operator[](int i) noexcept
    {
        return (i == 0) ? x : (i == 1) ? y
                                       : z;
    }

    constexpr vec3 &operator+=(const vec3 &v) noexcept
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    constexpr vec3 &operator*=(real t) noexcept
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    constexpr vec3 &operator/=(real t) noexcept { return *this *= (1.0f / t); }

    [[nodiscard]] constexpr real length() const noexcept
    {
        return std::sqrt(length_squared());
    }
    [[nodiscard]] constexpr real length_squared() const noexcept
    {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] constexpr bool near_zero() const noexcept
    {
        static constexpr real s = 1e-4f;
        return (std::abs(x) < s) && (std::abs(y) < s) && (std::abs(z) < s);
    }

    // Random vector generation
    [[nodiscard]] static vec3 random()
    {
        return {random_real(), random_real(), random_real()};
    }

    [[nodiscard]] static vec3 random(real min, real max)
    {
        return {random_real(min, max), random_real(min, max), random_real(min, max)};
    }
};

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;  // RGB color

// Utility Functions

[[nodiscard]] constexpr vec3 operator+(const vec3 &u, const vec3 &v) noexcept
{
    return {u.x + v.x, u.y + v.y, u.z + v.z};
}

[[nodiscard]] constexpr vec3 operator-(const vec3 &u, const vec3 &v) noexcept
{
    return {u.x - v.x, u.y - v.y, u.z - v.z};
}

[[nodiscard]] constexpr vec3 operator*(const vec3 &u, const vec3 &v) noexcept
{
    return {u.x * v.x, u.y * v.y, u.z * v.z};
}

[[nodiscard]] constexpr vec3 operator*(real t, const vec3 &v) noexcept
{
    return {t * v.x, t * v.y, t * v.z};
}

[[nodiscard]] constexpr vec3 operator*(const vec3 &v, real t) noexcept
{
    return t * v;
}

[[nodiscard]] constexpr vec3 operator/(const vec3 &v, real t) noexcept
{
    return (1.0f / t) * v;
}

[[nodiscard]] constexpr real dot(const vec3 &u, const vec3 &v) noexcept
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

[[nodiscard]] constexpr vec3 cross(const vec3 &u, const vec3 &v) noexcept
{
    return {u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

[[nodiscard]] constexpr vec3 unit_vector(const vec3 &v) noexcept
{
    return v / v.length();
}

[[nodiscard]] inline vec3 random_in_unit_disk() noexcept
{
    while (true)
    {
        // Create a random point in the [-1,1] square on the XY plane
        vec3 p{random_real(-1.0f, 1.0f), random_real(-1.0f, 1.0f), 0.0f};
        if (p.length_squared() < 1.0f)
            return p;
    }
}

[[nodiscard]] inline vec3 random_unit_vector() noexcept
{
    while (true)
    {
        auto p = vec3::random(-1.0f, 1.0f);
        auto lensq = p.length_squared();
        // Use float-safe epsilon (FLT_MIN is around 1e-38)
        // to avoid division by zero for points extremely close to the origin.
        if (1e-24f < lensq && lensq <= 1.0f)
            return p / std::sqrt(lensq);
    }
}

[[nodiscard]] inline vec3 random_on_hemisphere(const vec3 &normal) noexcept
{
    vec3 on_unit_sphere = random_unit_vector();
    // Check if the random vector is in the same hemisphere as the normal
    return (dot(on_unit_sphere, normal) > 0.0f) ? on_unit_sphere : -on_unit_sphere;
}

[[nodiscard]] constexpr vec3 reflect(const vec3 &v, const vec3 &n) noexcept
{
    return v - 2.0f * dot(v, n) * n;
}

[[nodiscard]] constexpr vec3 refract(const vec3 &uv, const vec3 &n, real etai_over_etat) noexcept
{
    auto cos_theta = std::min(dot(-uv, n), 1.0f);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::abs(1.0f - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}