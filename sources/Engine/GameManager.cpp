#include "GameManager.hpp"
#include "EngineUtil.hpp"
#include "Graphics/GraphicsUtil.hpp"
#include <thread>
#include <chrono>
#include "platform.hpp"

#include <iostream>

GLFWwindow* game_manager::game_window = nullptr;
std::unique_ptr<i_scene> game_manager::m_current_scene_ = nullptr;

void game_manager::init()
{
    glfwSetErrorCallback(engine_util::error_callback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    game_window = glfwCreateWindow(settings::window_width, settings::window_height, "Game Window", nullptr, nullptr);
    if (!game_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(game_window);
    glfwSetKeyCallback(game_window, engine_util::key_callback);
    glfwSetFramebufferSizeCallback(game_window, graphics_util::framebuffer_size_callback);
    glfwSwapInterval(0);
    gladLoadGL(glfwGetProcAddress);
    glClearColor(1, 1, 1, 1);
    m_current_scene_->init();
}

void game_manager::update(float dt)
{
    m_current_scene_->update(dt);
    m_current_scene_->late_update(dt);
}

void game_manager::render()
{
    m_current_scene_->render();
}

void game_manager::shutdown()
{
    m_current_scene_->shutdown();
    glfwDestroyWindow(game_window);
    glfwTerminate();
}


// Update the game_loop function to limit FPS
void game_manager::game_loop()
{
    const double fps_limit = 1.0 / settings::max_fps;
    double delta_time = 0.0f;
    double last_frame = 0.0f;
    double elapsed_time = 0.0f;
    int frame_count = 0;

    while (!glfwWindowShouldClose(game_window))
    {
        double current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        elapsed_time += delta_time;
        frame_count++;

        if (elapsed_time >= 1.0)
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
        if (frame_time < fps_limit)
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(fps_limit - frame_time));
        }
    }
}
GLFWwindow *game_manager::get_glfw_window()
{
    return game_window;
}