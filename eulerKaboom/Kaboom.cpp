#include "lib/header.h"

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "lib/geometry.h"

using Point3F = Point3<float>;

const float sphere_radius = 1.5;
const float noise_amplitude = 1.0;

template <typename T>
inline T lerp(const T &v0, const T &v1, float t) {
    return v0 + (v1 - v0) * std::max(0.f, std::min(1.f, t));
}

float texture(const float n) {
    float x = sin(n) * 43758.5453f;
    return x - floor(x);
}

float noise(const Point3F &x) {
    Point3F p(floor(x.x), floor(x.y), floor(x.z));
    Point3F f(x.x - p.x, x.y - p.y, x.z - p.z);
    f = f * (f * (Point3F(3.f, 3.f, 3.f) - f * 2.f));
    float n = p * Point3F(1.f, 57.f, 113.f);
    return lerp(lerp(lerp(texture(n + 0.f), texture(n + 1.f), f.x), lerp(texture(n + 57.f), texture(n + 58.f), f.x), f.y),
                lerp(lerp(texture(n + 113.f), texture(n + 114.f), f.x), lerp(texture(n + 170.f), texture(n + 171.f), f.x), f.y),
                f.z);
}

Point3F rotate(const Point3F &v) {
    return Point3F(Point3F(0.00, 0.80, 0.60) * v, Point3F(-0.80, 0.36, -0.48) * v, Point3F(-0.60, -0.48, 0.64) * v);
}

float fractal_brownian_motion(const Point3F &x) {
    Point3F p = rotate(x);
    float f = 0;
    f += 0.5000 * noise(p);
    p = p * 2.32f;
    f += 0.2500 * noise(p);
    p = p * 3.03f;
    f += 0.1250 * noise(p);
    p = p * 2.61f;
    f += 0.0625 * noise(p);
    return f / 0.9375;
}

Point3F palette_fire(const float d) {
    static const Point3F yellow(1.7, 1.3, 1.0);  // note that the color is "hot", i.e. has components >1
    static const Point3F orange(1.0, 0.6, 0.0);
    static const Point3F red(1.0, 0.0, 0.0);
    static const Point3F darkgray(0.2, 0.2, 0.2);
    static const Point3F gray(0.4, 0.4, 0.4);

    float x = std::max(0.f, std::min(1.f, d));
    if (x < .25f)
        return lerp(gray, darkgray, x * 4.f);
    else if (x < .5f)
        return lerp(darkgray, red, x * 4.f - 1.f);
    else if (x < .75f)
        return lerp(red, orange, x * 4.f - 2.f);
    return lerp(orange, yellow, x * 4.f - 3.f);
}

float signed_distance(const Point3F &p) {
    float displacement = -fractal_brownian_motion(p * 3.4f) * noise_amplitude;
    return p.norm() - (sphere_radius + displacement);
}

bool sphere_trace(const Point3F &orig, const Point3F &dir, Point3F &pos) {
    if (orig * orig > sqr(sphere_radius) + sqr(orig * dir)) {
        return false;  // early discard
    }

    pos = orig;
    for (size_t i = 0; i < 512; i++) {
        float d = signed_distance(pos);
        if (d < 0) {
            return true;
        }
        pos = pos + dir * std::max(d * 0.05f, .005f);
    }
    return false;
}

Point3F distance_field_normal(const Point3F &pos) {
    const float eps = 0.1;
    float d = signed_distance(pos);
    float nx = signed_distance(pos + Point3F(eps, 0, 0)) - d;
    float ny = signed_distance(pos + Point3F(0, eps, 0)) - d;
    float nz = signed_distance(pos + Point3F(0, 0, eps)) - d;
    return Point3F(nx, ny, nz).normalize();
}

int main() {
    const int width = 1920;
    const int height = 1080;
    const float fov = M_PI / 3.;
    std::vector<Point3F> framebuffer(width * height);

#pragma omp parallel for
    for (size_t j = 0; j < height; j++) {  // actual rendering loop
        for (size_t i = 0; i < width; i++) {
            float dir_x = (i + 0.5) - width / 2.;
            float dir_y = -(j + 0.5) + height / 2.;  // this flips the image at the same time
            float dir_z = -height / (2. * tan(fov / 2.));
            Point3F hit;
            if (sphere_trace(Point3F(0, 0, 3), Point3F(dir_x, dir_y, dir_z).normalize(),
                             hit)) {  // the camera is placed to (0,0,3) and it looks along the -z axis
                float noise_level = (sphere_radius - hit.norm()) / noise_amplitude;
                Point3F light_dir = (Point3F(10, 10, 10) - hit).normalize();  // one light is placed to (10,10,10)
                float light_intensity = std::max(0.4f, light_dir * distance_field_normal(hit));
                framebuffer[i + j * width] = palette_fire((-.2 + noise_level) * 2) * light_intensity;
            } else {
                framebuffer[i + j * width] = Point3F(0.2, 0.7, 0.8);  // background color
            }
        }
    }

    std::ofstream ofs("./out.ppm", std::ios::binary);  // save the framebuffer to file
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height * width; ++i) {
        for (size_t j = 0; j < 3; j++) {
            ofs << (char)(std::max(0, std::min(255, static_cast<int>(255 * framebuffer[i][j]))));
        }
    }
    ofs.close();

    return 0;
}
