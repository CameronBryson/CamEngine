#include "EngineUtil.hpp"
void engine_util::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
void engine_util::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	glfwSetWindowShouldClose(window, GLFW_TRUE);
    if( action == GLFW_PRESS )
    {
	EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyPressEvent(key));
    }
    if( action == GLFW_RELEASE )
    {
	EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyRelease(key));
    }
}
void engine_util::mouse_key_callback(GLFWwindow* window, int button, int action, int mods)
{
    if( action == GLFW_PRESS )
    {
	EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyPressEvent(button));
    }
    if( action == GLFW_RELEASE )
    {
	EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyRelease(button));
    }
}
bool engine_util::is_key_release(int key)
{
    if( glfwGetKey(game_manager::get_glfw_window(), key) == GLFW_RELEASE )
    {
	return true;
    }
    return false;
}
bool engine_util::is_key_pressed(int key)
{
    if( glfwGetKey(game_manager::get_glfw_window(), key) == GLFW_PRESS )
    {
	return true;
    }
    return false;
}
bool engine_util::is_mouse_button_release(int button)
{
    if( glfwGetMouseButton(game_manager::get_glfw_window(), button) == GLFW_RELEASE )
    {
	return true;
    }
    return false;
}
bool engine_util::is_mouse_button_pressed(int button)
{
    if( glfwGetMouseButton(game_manager::get_glfw_window(), button) == GLFW_PRESS )
    {
	return true;
    }
    return false;
}
std::string engine_util::build_path(const std::string& path)
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
