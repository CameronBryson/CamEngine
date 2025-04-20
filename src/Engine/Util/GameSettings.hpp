#pragma once
#include <glm/vec3.hpp>

namespace settings
{

    inline int window_width = 2500;
    inline int window_height = 1400;
    inline float aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
    constexpr unsigned max_fps = 9000;

} // namespace Settings
