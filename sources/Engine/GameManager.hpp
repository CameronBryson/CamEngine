#pragma once
#include "IScene.hpp"
#include "platform.hpp"
#include "GLFW/glfw3.h"


class GameManager
{
public:
    static void init();
    static void update(float dt);
    static void render();
    static void shutdown();
    static void gameLoop();

    template <typename T>
    static void loadScene()
    {
        if (m_current_scene_)
            shutdown();
        m_current_scene_ = std::make_unique<T>();
    }

    static GLFWwindow* get_glfw_window();
    static void set_glfw_window(GLFWwindow* window);

private:
    static GLFWwindow* game_window;
    static std::unique_ptr<IScene> m_current_scene_;
};
