#pragma once
#include <filesystem>

#include "GameManager.hpp"
#include "EventHandler.hpp"

#include <GLFW/glfw3.h>

class engine_util
{
public:
  static void error_callback(int error, const char* description);

  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void mouse_key_callback(GLFWwindow* window, int button, int action, int mods);
  static bool is_key_release(int key);
  static bool is_key_pressed(int key);
  static bool is_mouse_button_release(int button);
  static bool is_mouse_button_pressed(int button);
  static std::string build_path(const std::string& path);
};
