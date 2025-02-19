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


void OpenGlUtil::init()
{
	glfwSetErrorCallback(engine_util::errorCallback);
	if( ! glfwInit() )
	{
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* game_window = glfwCreateWindow(settings::window_width, settings::window_height, "Game Window", nullptr, nullptr);
	if( ! game_window )
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(game_window);
	glfwSetKeyCallback(game_window, engine_util::keyCallback);
    glfwSetCursorPosCallback(game_window, engine_util::cursorPosCallback); // Add this line
	glfwSetMouseButtonCallback(game_window, engine_util::mouseKeyCallback);

	glfwSetFramebufferSizeCallback(game_window, OpenGlUtil::framebufferSizeCallback);
	glfwSwapInterval(0);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}


    glClearColor(0, 0, 0, 0);


    glEnable(GL_DEPTH_TEST);


	GameManager::set_glfw_window(game_window);
}

void OpenGlUtil::shutdown()
{
	glfwDestroyWindow(GameManager::mGameWindow);
	glfwTerminate();
}


void OpenGlUtil::clearBackground()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
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
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,

           -1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

           -1.0f, -1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,

           -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f, -1.0f,

           -1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        // Fill buffer
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Link vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }

    // Draw the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

}


void OpenGlUtil::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}



