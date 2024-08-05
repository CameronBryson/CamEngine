#include "GameManager.hpp"
game_manager *game_manager::m_instance_ = nullptr;
game_manager::game_manager()
{
    m_current_scene_ = nullptr;
}

game_manager::~game_manager()
{
    shutdown();
}

game_manager *game_manager::get_instance()
{
    if (m_instance_ == nullptr)
    {
        m_instance_ = new game_manager();
    }
    return m_instance_;
}


void game_manager::init()
{
    if(!glfwInit())
    {
       exit(EXIT_FAILURE);
    }
    game_window = glfwCreateWindow(settings::window_width,settings::window_height,"Game Window", nullptr,nullptr);
    if(!game_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(game_window);
    glfwSwapInterval(1);
    m_current_scene_->init();
}

void game_manager::update(const float dt) const
{
    m_current_scene_->update(dt);
    m_current_scene_->late_update(dt);
}

void game_manager::render() const
{
    m_current_scene_->render();
}

void game_manager::shutdown()
{
    m_current_scene_->shutdown();
    delete m_current_scene_;
    // delete m_instance;
}
void game_manager::game_loop()
{
    int count = 0;
    while(!glfwWindowShouldClose(game_window))
    {
        glViewport(0,0,settings::window_width, settings::window_height);
        update(0.0f);
        render();
        glfwSwapBuffers(game_window);
        glfwPollEvents();
        count++;
        printf("%d\n", count);
    }
}
float game_manager::get_delta_time() const
{
    return delta_time_;
}