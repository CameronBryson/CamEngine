#pragma once
#include "IScene.hpp"
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
    if (m_current_scene_ != nullptr)
      shutdown();
    m_current_scene_ = new T();
  }
  static GLFWwindow* get_glfw_window();

private:
  static GLFWwindow* game_window;
  static i_scene* m_current_scene_;
};