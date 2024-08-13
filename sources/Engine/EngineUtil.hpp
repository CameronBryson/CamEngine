#pragma once
#include <filesystem>

#include "GameManager.hpp"

#include <GLFW/glfw3.h>

class engine_util
{
public:
    static void error_callback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    static bool is_key_pressed(int key)
    {
        if (glfwGetKey(game_manager::get_glfw_window(), key) == GLFW_PRESS)
        {
            return true;
        }
        return false;
    }
    static std::string build_path(const std::string& path)
    {
        auto current_path = std::filesystem::current_path();
        auto parent_path = current_path.parent_path().parent_path();
        return (parent_path / path).make_preferred().string();
    }
};
