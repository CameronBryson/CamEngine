#pragma once
#include "Engine/Util/platform.hpp"
#include <memory>

#include "alc.h"


class BaseScene;


class GameManager
{
public:
    static void init();
    static void update(float dt);
    static void render();
    static void shutdown();
    static void gameLoop();

    template <typename T>
    static void loadScene()
    {
        if (mCurrentScene)
            shutdown();
        mCurrentScene = std::make_unique<T>();
    }

    static GLFWwindow* get_glfw_window();
    static void set_glfw_window(GLFWwindow* window);

	static ALCdevice* get_audio_device();
	static ALCcontext* get_audio_context();

	static void set_audio_device(ALCdevice* device);
	static void set_audio_context(ALCcontext* context);

private:
    static GLFWwindow* mGameWindow;
	static ALCdevice* mAudioDevice;
	static ALCcontext* mAudioContext;
    static std::unique_ptr<BaseScene> mCurrentScene;
};
