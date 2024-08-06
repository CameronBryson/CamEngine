#include "GameManager.hpp"
#include "EngineUtil.hpp"
#include "Graphics/GraphicsUtil.hpp"

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
    gladLoadGL();
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

void game_manager::game_loop()
{
    const double fps_limit = 1.0 / settings::max_fps;
    double last_update_time = glfwGetTime();
    double last_frame_time = glfwGetTime();
    double last_fps_time = glfwGetTime();
    int frame_count = 0;

    while (!glfwWindowShouldClose(game_window))
    {
        double now = glfwGetTime();
        double delta_time = now - last_update_time;

        if ((now - last_frame_time) >= fps_limit)
        {
            update(static_cast<float>(delta_time));
            render();
            glfwSwapBuffers(game_window);
            last_frame_time = now;
            frame_count++;
        }

        if ((now - last_fps_time) >= 1.0)
        {
            double fps = frame_count / (now - last_fps_time);
            printf("FPS: %f\n", fps);
            frame_count = 0;
            last_fps_time = now;
        }

        last_update_time = now;
        glfwPollEvents();
    }
}
GLFWwindow *game_manager::get_glfw_window()
{
    return game_window;
}