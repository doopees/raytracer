#pragma once

#include "../external/stb_image_write.h"

#include "common.h"
#include "hittable.h"
#include "material.h"
#include "bvh_node.h"

#include <vector>
#include <execution>
#include <algorithm>
#include <print>
#include <filesystem>
#include <fstream>

class camera
{
public:
    real aspect_ratio = 1.0;    // Ratio of image width over height
    int image_width = 100;      // Rendered image width in pixel count
    int samples_per_pixel = 10; // Count of random samples for each pixel
    int max_depth = 10;         // Maximum number of ray bounces into scene

    real vfov = 90.0;                  // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
    point3 lookat = point3(0, 0, -1);  // Point camera is looking at
    vec3 vup = vec3(0, 1, 0);          // Camera-relative "up" direction

    real defocus_angle = 0; // Variation angle of rays through each pixel
    real focus_dist = 10;   // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable_list &world, std::string_view filename = "render.png")
    {
        initialize();
        auto world_bvh = std::make_shared<bvh_node>(world);
        std::vector<Pixel> pixels(image_width * image_height);

        // Generate tiles for parallel rendering
        const int tile_size = 16;
        auto tiles = generate_tiles(tile_size);

        std::atomic<uint64_t> total_rays{0};
        auto start_time = std::chrono::high_resolution_clock::now();

        // Core render loop
        std::for_each(std::execution::par, tiles.begin(), tiles.end(),
                      [this, &world_bvh, &pixels, &total_rays](const Tile &tile)
                      {
                          total_rays += render_tile(tile, *world_bvh, pixels);
                      });

        auto end_time = std::chrono::high_resolution_clock::now();

        // Save the image, then report and log results
        auto full_path = save_image(pixels, filename);
        report_results(full_path, start_time, end_time, total_rays.load());
    }

private:
    int image_height;         // Rendered image height
    real pixel_samples_scale; // Color scale factor for a sum of pixel samples
    point3 center;            // Camera center
    point3 pixel00_loc;       // Location of pixel 0, 0
    vec3 pixel_delta_u;       // Offset to pixel to the right
    vec3 pixel_delta_v;       // Offset to pixel below
    vec3 u, v, w;             // Camera frame basis vectors
    vec3 defocus_disk_u;      // Defocus disk horizontal radius
    vec3 defocus_disk_v;      // Defocus disk vertical radius

    struct Tile
    {
        int x_start, y_start, width, height;
    };

    void initialize()
    {
        // Calculate height and ensure it's at least 1
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        // Precompute the multiplier for averaging samples
        pixel_samples_scale = 1.0f / samples_per_pixel;

        // Camera positioning
        center = lookfrom;

        // Determine viewport dimensions
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2.0f);
        auto viewport_height = 2.0f * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<real>(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Vectors across the viewport edges
        auto viewport_u = viewport_width * u;   // Vector across viewport horizontal edge
        auto viewport_v = viewport_height * -v; // Vector down viewport vertical edge

        // Delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / static_cast<real>(image_width);
        pixel_delta_v = viewport_v / static_cast<real>(image_height);

        // Calculate the location of the upper-left pixel (Pixel 0,0)
        // Viewport is at z = -focal_length (looking down -Z axis)
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0f - viewport_v / 2.0f;
        pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2.0f));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    std::vector<Tile> generate_tiles(const int tile_size) const
    {
        // Generate tiles for parallel rendering
        std::vector<Tile> tiles;
        for (int y = 0; y < image_height; y += tile_size)
        {
            for (int x = 0; x < image_width; x += tile_size)
            {
                tiles.push_back({x, y,
                                 std::min(tile_size, image_width - x),
                                 std::min(tile_size, image_height - y)});
            }
        }
        return tiles;
    }

    uint64_t render_tile(const Tile &tile, const hittable &world, std::vector<Pixel> &pixels) const
    {
        // Render a single tile and return the number of rays traced
        uint64_t rays_traced = 0;
        for (int j = tile.y_start; j < tile.y_start + tile.height; ++j)
        {
            for (int i = tile.x_start; i < tile.x_start + tile.width; ++i)
            {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s)
                {
                    pixel_color += ray_color(get_ray(i, j), max_depth, world);
                    rays_traced++;
                }
                pixels[j * image_width + i] = to_pixel(pixel_color * pixel_samples_scale);
            }
        }
        return rays_traced;
    }

    std::filesystem::path save_image(const std::vector<Pixel> &pixels, std::string_view filename) const
    {
        // Ensure images directory exists
        std::filesystem::path dir("images");
        if (!std::filesystem::exists(dir))
            std::filesystem::create_directory(dir);

        // Save image using stb_image_write
        auto full_path = dir / filename;
        stbi_write_png(full_path.string().c_str(), image_width, image_height, 3,
                       pixels.data(), image_width * 3);
        return full_path;
    }

    void report_results(const std::filesystem::path &path,
                        auto start, auto end, uint64_t total_rays) const
    {
        // Calculate elapsed time and rays per second
        std::chrono::duration<float> elapsed = end - start;
        double mrays_s = (total_rays / elapsed.count()) / 1'000'000.0;

        // Print results to console
        std::println(stderr, "Done: {} | {:.2f}s | {:.2f} MRays/s",
                     path.string(), elapsed.count(), mrays_s);

        // Log performance data to CSV
        log_performance(path, elapsed.count(), total_rays, mrays_s);
    }

    void log_performance(const std::filesystem::path &path, float elapsed, uint64_t rays, double mrays_s) const
    {
        std::ofstream log("perf_log.csv", std::ios::app);

        // Check if file is empty
        if (std::filesystem::exists("perf_log.csv") && std::filesystem::file_size("perf_log.csv") == 0)
        {
            log << "Timestamp,File,Seconds,TotalRays,MRays_s\n";
        }

        // Write performance data to log
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        log << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << ","
            << path.string() << ","
            << elapsed << ","
            << rays << ","
            << mrays_s << "\n";
    }

    [[nodiscard]] ray get_ray(int i, int j) const
    {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x) * pixel_delta_u) + ((j + offset.y) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0.0f) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    [[nodiscard]] vec3 sample_square() const
    {
        // Returns a vector to a random point in the [-.5, .5] unit square
        return {random_real() - 0.5f, random_real() - 0.5f, 0.0f};
    }

    [[nodiscard]] point3 defocus_disk_sample() const noexcept
    {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
    }

    [[nodiscard]] constexpr color ray_color(const ray &r, int depth, const hittable &world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0.0f, 0.0f, 0.0f);

        hit_record rec;
        if (world.hit(r, interval(0.001f, infinity), rec))
        {
            ray scattered;
            color attenuation;
            color color_from_emission = rec.mat->emitted();

            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return color_from_emission + (attenuation * ray_color(scattered, depth - 1, world));
            else
                return color_from_emission;
        }

        // Background gradient (sky)
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - a) * color(1.0f, 1.0f, 1.0f) + a * color(0.5f, 0.7f, 1.0f);
    }
};