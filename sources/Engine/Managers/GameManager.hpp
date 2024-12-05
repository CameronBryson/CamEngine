#pragma once
#include <memory>

#include "Engine/Util/platform.hpp"
#include "alc.h"
class GraphicsManager;
class AudioManager;
class BaseScene;


class GameManager
{
public:
	static void firstInit();
	static void init();
	static void update(float dt);
	static void render();
	static void shutdown();
	static void finalShutdown();
	static void gameLoop();

	template <typename T>
	static void loadScene()
	{
		if (mCurrentScene)
			shutdown();
		mCurrentScene = std::make_unique<T>();
		init();
	}

	static GLFWwindow* get_glfw_window();
	static void set_glfw_window(GLFWwindow* window);

	static ALCdevice* get_audio_device();
	static ALCcontext* get_audio_context();

	static void set_audio_device(ALCdevice* device);
	static void set_audio_context(ALCcontext* context);


	static GLFWwindow* mGameWindow;
	static ALCdevice* mAudioDevice;
	static ALCcontext* mAudioContext;
	static std::unique_ptr<BaseScene> mCurrentScene;
	static std::unique_ptr<AudioManager> mAudioManager;
	static std::unique_ptr<GraphicsManager> mGraphicsManager;
};
