#pragma once
#include <cmath>
#include <cstdlib>

class MathUtil
{
  public:
    // Constants
    static constexpr float pi = 3.14159265358979323846f;
    static constexpr float two_pi = 6.28318530717958647692f;
    static constexpr float half_pi = 1.57079632679489661923f;
    static constexpr float deg_to_rad = pi / 180.0f;
    static constexpr float rad_to_deg = 180.0f / pi;

    // Utility functions
    static float clamp(const float value, const float min, const float max)
    {
        return std::fmax(min, std::fmin(max, value));
    }

    static float lerp(const float a, const float b, const float t)
    {
        return a + t * (b - a);
    }

    static float distance(const float x1, const float y1, const float x2, const float y2)
    {
        const float dx = x2 - x1;
        const float dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }

    static float to_radians(const float degrees)
    {
        return degrees * deg_to_rad;
    }

    static float to_degrees(const float radians)
    {
        return radians * rad_to_deg;
    }

    static float random_float(const float min, const float max)
    {
        return min + static_cast<float>(rand()) / (static_cast<float>(static_cast<float>(RAND_MAX) / (max - min)));
    }

    static float smooth_step(const float edge0, const float edge1, float x)
    {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * (3 - 2 * x);
    }

    static float smoother_step(const float edge0, const float edge1, float x)
    {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    static float sign(const float value)
    {
        return (value > 0) - (value < 0);
    }

    static float max(const float a, const float b)
    {
        return (a > b) ? a : b;
    }

    static float min(const float a, const float b)
    {
        return (a < b) ? a : b;
    }

    // New utility functions
    static float pow(const float base, const float exponent)
    {
        return std::pow(base, exponent);
    }

    static float sqrt(const float value)
    {
        return std::sqrt(value);
    }

    static float abs(const float value)
    {
        return std::fabs(value);
    }

};
