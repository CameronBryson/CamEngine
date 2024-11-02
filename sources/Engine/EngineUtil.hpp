#pragma once
#include <filesystem>

#include "GameManager.hpp"
#include "EventHandler.hpp"

#include <GLFW/glfw3.h>

class engine_util
{
public:
  static void errorCallback(int error, const char* description);

  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void mouseKeyCallback(GLFWwindow* window, int button, int action, int mods);
  static bool isKeyReleased(int key);
  static bool isKeyPressed(int key);
  static bool isMouseButtonReleased(int button);
  static bool isMouseButtonPressed(int button);
  static std::string buildPath(const std::string& path);
};
