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

    // Save GL state
    GLint last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // positions          // texture coords
            // Back face
           -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
           -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
           -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,

           // Left face
          -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
          -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
          -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
          -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
          -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
          -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

          // Right face
          1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
          1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
          1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
          1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
          1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
          1.0f, -1.0f, -1.0f,  0.0f, 0.0f,

          // Front face
         -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
          1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
          1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
          1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
         -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,

         // Top face
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,

        // Bottom face
       -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
       -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
       -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  1.0f, 0.0f
        };
        GL_CHECK(glGenVertexArrays(1, &cubeVAO));
        GL_CHECK(glGenBuffers(1, &cubeVBO));

        GL_CHECK(glBindVertexArray(cubeVAO));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
            vertices, GL_STATIC_DRAW));

        // Position attribute
        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            5 * sizeof(float), (void*)0));

        // Texture coord attribute
        GL_CHECK(glEnableVertexAttribArray(1));
        GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            5 * sizeof(float), (void*)(3 * sizeof(float))));

        // Unbind VAO first, then VBO
        GL_CHECK(glBindVertexArray(0));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    // Draw
    GL_CHECK(glBindVertexArray(cubeVAO));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 36));

    // Restore state
    GL_CHECK(glBindVertexArray(last_vertex_array));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer));
}

void OpenGlUtil::checkGLState()
{
    GLint maxTextureUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

    GLint activeProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgram);
    std::cout << "Active Shader Program: " << activeProgram << "\n";

    GLint activeTexture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
    std::cout << "Active Texture Unit: " << (activeTexture - GL_TEXTURE0) << "\n";

    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    std::cout << "Blend Enabled: " << (blendEnabled ? "true" : "false") << "\n";

    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    std::cout << "Depth Test Enabled: " << (depthTestEnabled ? "true" : "false") << "\n";

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    std::cout << "Viewport: [" << viewport[0] << ", " << viewport[1] << ", " 
        << viewport[2] << ", " << viewport[3] << "]\n";

    // Check all texture units
    std::cout << "\nTexture Units State:\n";
    for(int i = 0; i < maxTextureUnits; i++) {
        chechTextureState(i);
    }

    GLint boundFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);
    std::cout << "\nBound Framebuffer: " << boundFBO << "\n";
}

void OpenGlUtil::chechTextureState(unsigned int unit)
{
    GLint activeUnit;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
    activeUnit -= GL_TEXTURE0;

    GLint boundTexture;
    glActiveTexture(GL_TEXTURE0 + unit);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);

    GLint compareMode;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, &compareMode);

    GLint compareFunc;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, &compareFunc);

    std::cout << "Texture Unit " << unit << " State:\n"
        << "  Bound Texture: " << boundTexture << "\n"
        << "  Compare Mode: " << (compareMode == GL_COMPARE_REF_TO_TEXTURE ? "GL_COMPARE_REF_TO_TEXTURE" : "GL_NONE") << "\n"
        << "  Compare Func: " << compareFunc << "\n";

    // Restore original active texture unit
    glActiveTexture(GL_TEXTURE0 + activeUnit);
}



void OpenGlUtil::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}



