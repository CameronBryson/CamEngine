#pragma once
#include "Engine/Util/platform.hpp"
#include <filesystem>
#include <unordered_map>

class engine_util {
public:
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseKeyCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    // Input state checking
    static bool isKeyPressed(int key);
    static bool isKeyReleased(int key);
    static bool isMouseButtonPressed(int button);
    static bool isMouseButtonReleased(int button);

    // Mouse position
    static void getMousePosition(double& x, double& y);
    static void getMouseDelta(double& dx, double& dy);
    static void setMouseCursorMode(int mode); // GLFW_CURSOR_NORMAL, GLFW_CURSOR_DISABLED, etc.

private:
    static std::unordered_map<int, bool> mKeyStates;
    static std::unordered_map<int, bool> mMouseButtonStates;
    static double mMouseX, mMouseY;
    static double mLastMouseX, mLastMouseY;
    static bool mFirstMouse;
};
