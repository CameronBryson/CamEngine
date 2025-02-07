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
	glfwSetMouseButtonCallback(game_window, engine_util::mouseKeyCallback);
	glfwSetFramebufferSizeCallback(game_window, OpenGlUtil::framebufferSizeCallback);
	glfwSwapInterval(0);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}


	glClearColor(1, 1, 1, 1);

	GameManager::set_glfw_window(game_window);
}

void OpenGlUtil::shutdown()
{
	glfwDestroyWindow(GameManager::mGameWindow);
	glfwTerminate();
}


void OpenGlUtil::clearBackground()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void OpenGlUtil::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}



