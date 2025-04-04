#pragma once
#include <glm/vec3.hpp>

namespace settings
{

    inline int window_width = 2500;
    inline int window_height = 1400;
    inline float aspect_ratio = (float) window_width / (float) window_height;
    constexpr unsigned max_fps = 9000;

} // namespace Settings
