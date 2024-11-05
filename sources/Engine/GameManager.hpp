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
        if (mCurrentScene)
            shutdown();
        mCurrentScene = std::make_unique<T>();
    }

    static GLFWwindow* get_glfw_window();
    static void set_glfw_window(GLFWwindow* window);

private:
    static GLFWwindow* mGameWindow;
    static std::unique_ptr<IScene> mCurrentScene;
};
