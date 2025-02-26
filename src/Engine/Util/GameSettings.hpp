#pragma once
#include <glm/vec3.hpp>

namespace settings
{

    inline int window_width = 1920;
    inline int window_height = 1080;
    inline float aspect_ratio = (float) window_width / (float) window_height;
    constexpr unsigned max_fps = 6000;
    constexpr unsigned max_entities = 10000;
    constexpr float EPSILON = 1e-6f;

} // namespace Settings
