#pragma once
namespace settings
{
    inline int window_width = 1920;
    inline int window_height = 1080;
    inline float aspect_ratio = (float) window_width / (float) window_height;
    constexpr unsigned max_fps = 300;
    constexpr unsigned max_entities = 10000;
} // namespace Settings
