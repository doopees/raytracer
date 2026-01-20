#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <chrono>
#include <fstream>
#include <print>
#include <string_view>
#include <vector>

[[nodiscard]] constexpr bool hit_sphere(const point3 &center, double radius, const ray &r)
{
    vec3 oc = center - r.origin();
    // Simplified quadratic: a*t^2 + 2ht + c = 0
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = h * h - a * c;

    return (discriminant >= 0);
}

[[nodiscard]] constexpr color ray_color(const ray &r)
{
    // Check if the ray hits a sphere at z = -1 with radius 0.5
    if (hit_sphere(point3(0, 0, -1), 0.5, r))
    {
        return color(1, 0, 0); // Render red if hit
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main()
{
    // --- Image Configuration ---
    constexpr auto aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 400;

    // Calculate height, ensure it's at least 1
    constexpr int image_height = []
    {
        int h = static_cast<int>(image_width / aspect_ratio);
        return (h < 1) ? 1 : h;
    }();

    constexpr std::string_view filename = "image.ppm";

    // --- Camera & Viewport Setup ---
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    // We use the actual image ratio to prevent stretching
    auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
    auto camera_center = point3(0, 0, 0);

    // Vectors across the viewport edges
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Delta vectors from pixel to pixel
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper-left pixel (Pixel 0,0)
    // Viewport is at z = -focal_length (looking down -Z axis)
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // --- Rendering ---
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<Pixel> pixels;
    pixels.reserve(image_width * image_height);

    for (int j = 0; j < image_height; ++j)
    {
        // Only update progress every 10 lines
        if (j % 10 == 0)
            std::print(stderr, "\rScanlines remaining: {:3} ", image_height - j);

        for (int i = 0; i < image_width; ++i)
        {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);
            color pixel_color = ray_color(r);
            pixels.push_back(to_pixel(pixel_color));
        }
    }

    // --- File Output (Binary P6) ---
    std::ofstream image_file{filename.data(), std::ios::binary};
    if (!image_file)
    {
        std::println(stderr, "Error: Could not open file for writing.");
        return 1;
    }

    std::println(image_file, "P6\n{} {}\n255", image_width, image_height);
    image_file.write(
        reinterpret_cast<const char *>(pixels.data()), pixels.size() * sizeof(Pixel));

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> duration = end_time - start_time;

    std::println(stderr, "\rDone.                       ");
    std::println(stderr, "Total render time: {:.2f}ms", duration.count());

    return 0;
}