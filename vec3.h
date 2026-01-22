#pragma once

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

    // Near-zero check
    [[nodiscard]] constexpr bool near_zero() const noexcept
    {
        static constexpr double s = 1e-8;
        return (std::abs(x) < s) && (std::abs(y) < s) && (std::abs(z) < s);
    }

    // Random vector generation
    [[nodiscard]] static vec3 random()
    {
        return {random_double(), random_double(), random_double()};
    }

    [[nodiscard]] static vec3 random(double min, double max)
    {
        return {random_double(min, max), random_double(min, max), random_double(min, max)};
    }
};

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;  // RGB color

// Utility Functions

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

[[nodiscard]] inline vec3 random_unit_vector()
{
    while (true)
    {
        auto p = vec3::random(-1, 1);
        auto lensq = p.length_squared();
        // 1e-160 is a tiny number to avoid division by zero for points
        // extremely close to the origin.
        if (1e-160 < lensq && lensq <= 1)
            return p / std::sqrt(lensq);
    }
}

[[nodiscard]] inline vec3 random_on_hemisphere(const vec3 &normal)
{
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

[[nodiscard]] constexpr vec3 reflect(const vec3 &v, const vec3 &n) noexcept
{
    return v - 2 * dot(v, n) * n;
}

[[nodiscard]] constexpr vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat) noexcept
{
    auto cos_theta = std::min(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::abs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}