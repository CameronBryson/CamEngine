#pragma once
#include "Color.hpp"
#include "Math/Vectors.hpp"

class graphics_util
{
public:
    static void draw_line(vec2 start, vec2 end, color color)
    {
    }
    static void draw_text(const char *text, int posX, int posY, int fontSize, color color)
    {

    }
    static int get_screen_width()
    {
        return 1920;
    }
    static int get_screen_height()
    {
        return 1080;
    }
    static void clear_background(color color)
    {

    }

};