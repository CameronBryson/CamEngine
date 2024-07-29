#pragma once
#include <cmath>
class vec2
{
  public:
    float x = 0, y = 0;

    vec2() = default;
    vec2(const float x, const float y) : x(x), y(y)
    {
    }

    vec2 operator+(const vec2 &other) const
    {
        return {x + other.x, y + other.y};
    }
    vec2 operator-(const vec2 &other) const
    {
        return {x - other.x, y - other.y};
    }
    vec2 operator*(const float scalar) const
    {
        return {x * scalar, y * scalar};
    }

    vec2 &operator+=(const vec2 &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    vec2 &operator-=(const vec2 &other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    vec2 &operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    [[nodiscard]] float length() const
    {
        return std::sqrt(x * x + y * y);
    }
    [[nodiscard]] float length_squared() const
    {
        return x * x + y * y;
    }
    [[nodiscard]] vec2 normalized() const
    {
        const float len = length();
        return len > 0 ? vec2(x / len, y / len) : vec2();
    }
};

class vec3
{
  public:
    float x = 0, y = 0, z = 0;

    vec3() = default;
    vec3(const float x, const float y, const float z) : x(x), y(y), z(z)
    {
    }

    vec3 operator+(const vec3 &other) const
    {
        return {x + other.x, y + other.y, z + other.z};
    }
    vec3 operator-(const vec3 &other) const
    {
        return {x - other.x, y - other.y, z - other.z};
    }
    vec3 operator*(const float scalar) const
    {
        return {x * scalar, y * scalar, z * scalar};
    }

    vec3 &operator+=(const vec3 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    vec3 &operator-=(const vec3 &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    vec3 &operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    [[nodiscard]] float length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    [[nodiscard]] float length_squared() const
    {
        return x * x + y * y + z * z;
    }
    [[nodiscard]] vec3 normalized() const
    {
        const float len = length();
        return len > 0 ? vec3(x / len, y / len, z / len) : vec3();
    }
};

class vec4
{
  public:
    float x = 0, y = 0, z = 0, w = 0;

    vec4() = default;
    vec4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w)
    {
    }

    vec4 operator+(const vec4 &other) const
    {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }
    vec4 operator-(const vec4 &other) const
    {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }
    vec4 operator*(const float scalar) const
    {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    vec4 &operator+=(const vec4 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    vec4 &operator-=(const vec4 &other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    vec4 &operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    [[nodiscard]] float length() const
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
    [[nodiscard]] float length_squared() const
    {
        return x * x + y * y + z * z + w * w;
    }
    [[nodiscard]] vec4 normalized() const
    {
        const float len = length();
        return len > 0 ? vec4(x / len, y / len, z / len, w / len) : vec4();
    }
};