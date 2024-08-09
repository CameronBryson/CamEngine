#pragma once
#include "IScene.hpp"
#include "platform.hpp"
#include "GLFW/glfw3.h"


class game_manager
{
public:
  static void init();
  static void update(float dt);
  static void render();
  static void shutdown();
  static void game_loop();
  template <typename T> static void load_scene()
  {
    if (m_current_scene_)
      shutdown();
    m_current_scene_ = std::make_unique<T>();
  }
  static GLFWwindow* get_glfw_window();

private:
  static GLFWwindow* game_window;
  static std::unique_ptr<i_scene> m_current_scene_;
};