#include "EngineUtil.hpp"

#include <Engine/Events/EventHandler.hpp>
#include <Engine/Managers/GameManager.hpp>

void engine_util::errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}
void engine_util::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	glfwSetWindowShouldClose(window, GLFW_TRUE);
	if( action == GLFW_PRESS )
	{
	EventHandler::GetInstance()->inputDispatcher.SendEvent(KeyPressEvent(key));
	}
	if( action == GLFW_RELEASE )
	{
	EventHandler::GetInstance()->inputDispatcher.SendEvent(KeyReleaseEvent(key));
	}
}
void engine_util::mouseKeyCallback(GLFWwindow* window, int button, int action, int mods)
{
	if( action == GLFW_PRESS )
	{
	EventHandler::GetInstance()->inputDispatcher.SendEvent(KeyPressEvent(button));
	}
	if( action == GLFW_RELEASE )
	{
	EventHandler::GetInstance()->inputDispatcher.SendEvent(KeyReleaseEvent(button));
	}
}
bool engine_util::isKeyReleased(int key)
{
	if( glfwGetKey(GameManager::get_glfw_window(), key) == GLFW_RELEASE )
	{
	return true;
	}
	return false;
}
bool engine_util::isKeyPressed(int key)
{
	if( glfwGetKey(GameManager::get_glfw_window(), key) == GLFW_PRESS )
	{
	return true;
	}
	return false;
}
bool engine_util::isMouseButtonReleased(int button)
{
	if( glfwGetMouseButton(GameManager::get_glfw_window(), button) == GLFW_RELEASE )
	{
	return true;
	}
	return false;
}
bool engine_util::isMouseButtonPressed(int button)
{
	if( glfwGetMouseButton(GameManager::get_glfw_window(), button) == GLFW_PRESS )
	{
	return true;
	}
	return false;
}
std::string engine_util::buildPath(const std::string& path)
{
//#ifdef _WIN32
	auto current_path = std::filesystem::current_path();
	auto parent_path = current_path.parent_path().parent_path().parent_path();
	return (parent_path / path).make_preferred().string();
//#else
	//return "../" + path;
	//return (std::filesystem::current_path() / path).string();
//#endif
}
