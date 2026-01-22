#pragma once

#include "common.h"
#include "hittable.h"
#include "material.h"

#include <vector>
#include <fstream>
#include <print>

class camera
{
public:
    double aspect_ratio = 1.0;         // Ratio of image width over height
    int image_width = 100;             // Rendered image width in pixel count
    int samples_per_pixel = 10;        // Count of random samples for each pixel
    int max_depth = 10;                // Maximum number of ray bounces into scene
    double vfov = 90;                  // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
    point3 lookat = point3(0, 0, -1);  // Point camera is looking at
    vec3 vup = vec3(0, 1, 0);          // Camera-relative "up" direction

    void render(const hittable &world, std::string_view filename = "output.ppm")
    {
        initialize();

        std::vector<Pixel> pixels;
        pixels.reserve(image_width * image_height);

        for (int j = 0; j < image_height; ++j)
        {
            // Only update progress every 10 lines
            if (j % 10 == 0)
                std::print(stderr, "\rScanlines remaining: {:3} ", image_height - j);

            for (int i = 0; i < image_width; ++i)
            {
                color pixel_color(0, 0, 0);

                // Multi-sample loop for antialiasing
                for (int sample = 0; sample < samples_per_pixel; ++sample)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                // Scale the summed color and convert to 8-bit Pixel
                pixels.push_back(to_pixel(pixel_color * pixel_samples_scale));
            }
        }

        save_binary_p6(filename, pixels);
        std::println(stderr, "\rDone.                       ");
    }

private:
    int image_height;           // Rendered image height
    double pixel_samples_scale; // Color scale factor for a sum of pixel samples
    point3 center;              // Camera center
    point3 pixel00_loc;         // Location of pixel 0, 0
    vec3 pixel_delta_u;         // Offset to pixel to the right
    vec3 pixel_delta_v;         // Offset to pixel below
    vec3 u, v, w;               // Camera frame basis vectors

    void initialize()
    {
        // Calculate height and ensure it's at least 1
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        // Precompute the multiplier for averaging samples
        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera positioning
        center = lookfrom;

        // Determine viewport dimensions
        auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focal_length;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Vectors across the viewport edges
        auto viewport_u = viewport_width * u;   // Vector across viewport horizontal edge
        auto viewport_v = viewport_height * -v; // Vector down viewport vertical edge

        // Delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper-left pixel (Pixel 0,0)
        // Viewport is at z = -focal_length (looking down -Z axis)
        auto viewport_upper_left = center - (focal_length * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    [[nodiscard]] ray get_ray(int i, int j) const
    {
        // Random offset within the pixel boundaries for random sampling
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x) * pixel_delta_u) + ((j + offset.y) * pixel_delta_v);

        return ray(center, pixel_sample - center);
    }

    [[nodiscard]] vec3 sample_square() const
    {
        // Returns a vector to a random point in the [-.5, .5] unit square
        return {random_double() - 0.5, random_double() - 0.5, 0};
    }

    [[nodiscard]] constexpr color ray_color(const ray &r, int depth, const hittable &world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0.0, 0.0, 0.0);

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec))
        {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0.0, 0.0, 0.0);
        }

        // Background gradient (Sky)
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    void save_binary_p6(std::string_view filename, const std::vector<Pixel> &pixels) const
    {
        std::ofstream file{filename.data(), std::ios::binary};

        if (!file)
        {
            std::println(stderr, "\nError: Could not open file '{}' for writing.", filename);
            return;
        }

        std::println(file, "P6\n{} {}\n255", image_width, image_height);
        file.write(reinterpret_cast<const char *>(pixels.data()), pixels.size() * sizeof(Pixel));
    }
};