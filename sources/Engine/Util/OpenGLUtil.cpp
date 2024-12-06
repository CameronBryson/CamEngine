#include "Engine/pch.hpp"
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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

void OpenGlUtil::drawLine(glm::vec2& start, glm::vec2& end, const glm::vec3& color)
{
	convertPointToScreen(start);
	convertPointToScreen(end);
	glBegin(GL_LINES);
	glColor3f(color.r, color.g, color.b);
	glVertex2f(start.x, start.y);
	glVertex2f(end.x, end.y);
	glEnd();
}

void OpenGlUtil::drawText(const char* text, int posX, int posY, int fontSize, glm::vec3& color)
{
}

void OpenGlUtil::clearBackground()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::vec2 OpenGlUtil::convertPointToScreen(glm::vec2& point)
{
	point.x = ((point.x / settings::window_width) * 2.0f) - 1.0f;
	point.y = ((point.y / settings::window_height) * 2.0f) - 1.0f;
	return point;
}

void OpenGlUtil::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

glm::vec2 OpenGlUtil::getWindowSize(){
	int width,height;
	glfwGetWindowSize(GameManager::get_glfw_window(), &width, &height);
	return {width,height};
}
glm::vec2 OpenGlUtil::getMousePos(){
	double x, y;
	glfwGetCursorPos(GameManager::get_glfw_window(),&x,&y);
	return {x,y};
}

void OpenGlUtil::enableBlend()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGlUtil::disableBlend()
{
	glDisable(GL_BLEND);
}


