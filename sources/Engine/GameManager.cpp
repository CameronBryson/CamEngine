#include "GameManager.hpp"
#include "EngineUtil.hpp"
#include "Graphics/OpenGLUtil.hpp"
#include <thread>
#include <chrono>
#include "platform.hpp"

#include <iostream>

GLFWwindow * GameManager::mGameWindow = nullptr;
std::unique_ptr<IScene> GameManager::mCurrentScene = nullptr;

void GameManager::init()
{
    OpenGlUtil::init();
    mCurrentScene->init();
}

void GameManager::update(float dt)
{
    mCurrentScene->update(dt);
    mCurrentScene->lateUpdate(dt);
}

void GameManager::render()
{
    mCurrentScene->render();
}

void GameManager::shutdown()
{
    mCurrentScene->shutdown();
    glfwDestroyWindow(mGameWindow);
    glfwTerminate();
}


// Update the game_loop function to limit FPS
void GameManager::gameLoop()
{
    const double fps_limit = 1.0 / settings::max_fps;
    double delta_time = 0.0f;
    double last_frame = 0.0f;
    double elapsed_time = 0.0f;
    int frame_count = 0;

    while( ! glfwWindowShouldClose(mGameWindow) )
    {
        double current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        elapsed_time += delta_time;
        frame_count++;

        if( elapsed_time >= 1.0 )
        {
            int fps = frame_count;
            std::cout << "FPS: " << fps << std::endl;
            frame_count = 0;
            elapsed_time = 0.0;
        }

        glfwPollEvents();
        update(static_cast<float>(delta_time));
        render();
        glfwSwapBuffers(mGameWindow);

        // Limit FPS
        double frame_time = glfwGetTime() - current_frame;
        if( frame_time < fps_limit )
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(fps_limit - frame_time));
        }
    }
}

GLFWwindow * GameManager::get_glfw_window()
{
    return mGameWindow;
}

void GameManager::set_glfw_window(GLFWwindow * window)
{
    mGameWindow = window;
}