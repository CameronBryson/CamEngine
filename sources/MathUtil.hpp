#pragma once
#include <cmath>
#include <cstdlib>
#include "Vectors.hpp"

class MathUtil {
public:
    // Constants
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float TWO_PI = 6.28318530717958647692f;
    static constexpr float HALF_PI = 1.57079632679489661923f;
    static constexpr float DEG_TO_RAD = PI / 180.0f;
    static constexpr float RAD_TO_DEG = 180.0f / PI;

    // Utility functions
    static float clamp(float value, float min, float max) {
        return std::fmax(min, std::fmin(max, value));
    }

    static float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    static float distance(float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }

    static float toRadians(float degrees) {
        return degrees * DEG_TO_RAD;
    }

    static float toDegrees(float radians) {
        return radians * RAD_TO_DEG;
    }

    static float randomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    }

    static float smoothStep(float edge0, float edge1, float x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * (3 - 2 * x);
    }

    static float smootherStep(float edge0, float edge1, float x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    static float sign(float value) {
        return (value > 0) - (value < 0);
    }

    static float max(float a, float b) {
        return (a > b) ? a : b;
    }

    static float min(float a, float b) {
        return (a < b) ? a : b;
    }

    // New utility functions
    static float pow(float base, float exponent) {
        return std::pow(base, exponent);
    }

    static float sqrt(float value) {
        return std::sqrt(value);
    }

    static float abs(float value) {
        return std::fabs(value);
    }

    static float dotProduct(const Vec2& v1, const Vec2& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }

    static Vec2 crossProduct(const Vec2& v1, const Vec2& v2) {
        return Vec2(v1.y * v2.x - v1.x * v2.y, v1.x * v2.y - v1.y * v2.x);
    }

    static Vec3 crossProduct(const Vec3& v1, const Vec3& v2) {
        return Vec3(
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x
        );
    }
};
