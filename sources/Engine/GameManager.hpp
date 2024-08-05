#pragma once
#include "IScene.hpp"
#include "GLFW/glfw3.h"
class game_manager
{
  public:
    ~game_manager();
    static game_manager *get_instance();
    void init();
    void update(float dt) const;
    void render() const;
    void shutdown();
    void game_loop();
    // idk if I should be using template
    // better solution is probably to pass interface
    template <typename T> void load_scene()
    {
        if (m_current_scene_ != nullptr)
            shutdown();
        m_current_scene_ = new T();
    }
    [[nodiscard]] float get_delta_time() const;
  protected:
    game_manager();

  private:
    GLFWwindow* game_window;
    float delta_time_;
    static game_manager *m_instance_;
    i_scene *m_current_scene_;
};