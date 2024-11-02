#include "GameManager.hpp"
#include "EngineUtil.hpp"
#include "Graphics/OpenGLUtil.hpp"
#include <thread>
#include <chrono>
#include "platform.hpp"

#include <iostream>

GLFWwindow * GameManager::game_window = nullptr;
std::unique_ptr<IScene> GameManager::m_current_scene_ = nullptr;

void GameManager::init()
{
    OpenGlUtil::init();
    m_current_scene_->init();
}

void GameManager::update(float dt)
{
    m_current_scene_->update(dt);
    m_current_scene_->lateUpdate(dt);
}

void GameManager::render()
{
    m_current_scene_->render();
}

void GameManager::shutdown()
{
    m_current_scene_->shutdown();
    glfwDestroyWindow(game_window);
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

    while( ! glfwWindowShouldClose(game_window) )
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
        glfwSwapBuffers(game_window);

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
    return game_window;
}

void GameManager::set_glfw_window(GLFWwindow * window)
{
    game_window = window;
}