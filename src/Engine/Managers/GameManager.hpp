#pragma once
#include "Engine/Util/platform.hpp"
#include <memory>
class GraphicsManager;
class BaseScene;


class GameManager
{
public:
	static void firstInit();
	static void init();
	static void update(float dt);
	static void render(float dt);
	static void shutdown();
	static void finalShutdown();
	static void gameLoop();
	static void initializeFileSystem();

	template <typename T>
	static void loadScene()
	{
		mPendingScene = std::make_unique<T>();
	}

	static GLFWwindow* get_glfw_window();
	static void set_glfw_window(GLFWwindow* window);




	static GLFWwindow* mGameWindow;
	static std::unique_ptr<BaseScene> mCurrentScene;
	static std::unique_ptr<BaseScene> mPendingScene;
	static std::unique_ptr<GraphicsManager> mGraphicsManager;
};
