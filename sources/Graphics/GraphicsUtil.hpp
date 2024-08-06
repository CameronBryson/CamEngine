#pragma once
#include "platform.hpp"
#include "Color.hpp"

#include <cstring>

class graphics_util
{
public:
    static void draw_line(glm::vec2 start, glm::vec2 end, color color)
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
    static void clear_background()
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }
    static glm::vec2 convert_point_to_screen(glm::vec2 &point)
    {
        point.x =  ((point.x / settings::window_width)*2.0f)-1.0f;
        point.y = ((point.y/settings::window_height)*2.0f)-1.0f;
        return point;
    }
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0,0,width,height);
    }

};