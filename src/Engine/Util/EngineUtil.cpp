#include "pch.hpp"
#include "EngineUtil.hpp"
#include "Engine/Managers/GameManager.hpp"

// Static member initialization
std::unordered_map<int, bool> engine_util::mKeyStates;
std::unordered_map<int, bool> engine_util::mMouseButtonStates;
double engine_util::mMouseX = 0.0;
double engine_util::mMouseY = 0.0;
double engine_util::mLastMouseX = 0.0;
double engine_util::mLastMouseY = 0.0;
bool engine_util::mFirstMouse = true;

void engine_util::errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void engine_util::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        mKeyStates[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        mKeyStates[key] = false;
    }

    // Handle escape key for window closing
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void engine_util::mouseKeyCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        mMouseButtonStates[button] = true;
    }
    else if (action == GLFW_RELEASE) {
        mMouseButtonStates[button] = false;
    }
}

void engine_util::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (mFirstMouse) {
        mLastMouseX = xpos;
        mLastMouseY = ypos;
        mFirstMouse = false;
    }

    mMouseX = xpos;
    mMouseY = ypos;
}

bool engine_util::isKeyPressed(int key) {
    return mKeyStates[key];
}

bool engine_util::isKeyReleased(int key) {
    return !mKeyStates[key];
}

bool engine_util::isMouseButtonPressed(int button) {
    return mMouseButtonStates[button];
}

bool engine_util::isMouseButtonReleased(int button) {
    return !mMouseButtonStates[button];
}

void engine_util::getMousePosition(double& x, double& y) {
    x = mMouseX;
    y = mMouseY;
}

void engine_util::getMouseDelta(double& dx, double& dy) {
    dx = mMouseX - mLastMouseX;
    dy = mMouseY - mLastMouseY;
    mLastMouseX = mMouseX;
    mLastMouseY = mMouseY;
}

void engine_util::setMouseCursorMode(int mode) {
    glfwSetInputMode(GameManager::getGLFWWindow(), GLFW_CURSOR, mode);
}
