#include "pch.hpp"

#include "OpenGLUtil.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include <stb_image.h>
#include "Engine/Util/GameSettings.hpp"
#include <iostream>

#include "Engine/Managers/GameManager.hpp"
#include "Engine/Util/EngineUtil.hpp"
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glad/glad.h>



void OpenGlUtil::init()
{
    glfwSetErrorCallback(engine_util::errorCallback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

#ifdef _DEBUG
    // Request a debug context if we're in debug mode
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* game_window = glfwCreateWindow(
        settings::window_width,
        settings::window_height,
        "Game Window",
        nullptr,
        nullptr
    );
    if (!game_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(game_window);
    glfwSetKeyCallback(game_window, engine_util::keyCallback);
    glfwSetCursorPosCallback(game_window, engine_util::cursorPosCallback);
    glfwSetMouseButtonCallback(game_window, engine_util::mouseKeyCallback);
    glfwSetFramebufferSizeCallback(game_window, OpenGlUtil::framebufferSizeCallback);

    // Disable vsync here if desired
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

#ifdef _DEBUG
    // Enable the debug output and set up a callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglDebugCallback, nullptr);

    // Filter out low severity and notification messages
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

    GL_CHECK(glClearColor(0, 0, 0, 0));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_CULL_FACE));
    GL_CHECK(glCullFace(GL_BACK));
    GL_CHECK(glFrontFace(GL_CCW));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendEquation(GL_FUNC_ADD));

    GameManager::set_glfw_window(game_window);
}

void OpenGlUtil::shutdown()
{
    glfwDestroyWindow(GameManager::mGameWindow);
    glfwTerminate();
}

void OpenGlUtil::clearBackground()
{
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void OpenGlUtil::drawQuad()
{
    static GLuint quadVAO = 0;
    static GLuint quadVBO = 0;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        GL_CHECK(glGenVertexArrays(1, &quadVAO));
        GL_CHECK(glGenBuffers(1, &quadVBO));
        GL_CHECK(glBindVertexArray(quadVAO));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, quadVBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),
            quadVertices, GL_STATIC_DRAW));

        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(float), (void*)0));

        GL_CHECK(glEnableVertexAttribArray(1));
        GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(float), (void*)(2 * sizeof(float))));
    }
    GL_CHECK(glBindVertexArray(quadVAO));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
    GL_CHECK(glBindVertexArray(0));
}

void OpenGlUtil::drawCube()
{
    static unsigned int cubeVAO = 0;
    static unsigned int cubeVBO = 0;
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             // ... rest of your existing cube data ...
        };
        GL_CHECK(glGenVertexArrays(1, &cubeVAO));
        GL_CHECK(glGenBuffers(1, &cubeVBO));
        GL_CHECK(glBindVertexArray(cubeVAO));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
            vertices, GL_STATIC_DRAW));
        // Configure vertex attributes as needed
    }
    GL_CHECK(glBindVertexArray(cubeVAO));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36)); // Example: 36 vertices
    GL_CHECK(glBindVertexArray(0));
}


void OpenGlUtil::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}



