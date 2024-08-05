#pragma once
#include "Color.hpp"
#include "Math/Vectors.hpp"

#include <GL/gl.h>

class graphics_util
{
public:
    static void draw_line(vec2 start, vec2 end, color color)
    {
        convert_point_to_screen(start);
        convert_point_to_screen(end);
        glBegin(GL_LINES);
        glColor3f(color.r,color.g,color.b);
        glVertex2f(start.x,start.y);
        glVertex2f(end.x,end.y);
        glEnd();
    }
    static void draw_text(const char *text, int posX, int posY, int fontSize, color color)
    {
    }
    static int get_screen_width()
    {
        return settings::window_width;
    }
    static int get_screen_height()
    {
        return settings::window_height;
    }
    static void clear_background(color color)
    {
        glClearColor(color.r,color.g,color.b,color.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    static vec2 convert_point_to_screen(vec2 &point)
    {
        point.x =  ((point.x / settings::window_width)*2.0f)-1.0f;
        point.y = ((point.y/settings::window_height)*2.0f)-1.0f;
        return point;
    }

};