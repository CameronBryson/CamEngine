#include "pch.hpp"
#include "GameManager.hpp"
#include "GraphicsManager.hpp"
#include <chrono>
#include <thread>

#include <iostream>

#include <Engine/Util/OpenGLUtil.hpp>

#include <BaseScene.hpp>
#include <GLFW/glfw3.h>
#include <memory>
#include <GameSettings.hpp>
GLFWwindow * GameManager::mGameWindow = nullptr;
std::unique_ptr<BaseScene> GameManager::mCurrentScene = nullptr;
std::unique_ptr<BaseScene> GameManager::mPendingScene = nullptr;

std::unique_ptr<GraphicsManager> GameManager::mGraphicsManager = nullptr;
void GameManager::firstInit()
{
	gl::init();
	//OpenALUtil::init();
	//mAudioManager = std::make_unique<AudioManager>();
	mGraphicsManager = std::make_unique<GraphicsManager>();
	mGraphicsManager->loadResources();
	//mAudioManager->loadResources();
}
void GameManager::init()
{
    mCurrentScene->init();
	mCurrentScene->lateInit();
}

void GameManager::update(float dt)
{
    mCurrentScene->update(dt);
    mCurrentScene->lateUpdate(dt);
}

void GameManager::render(float dt)
{
    mCurrentScene->render(dt);
	mCurrentScene->lateRender();
}

void GameManager::shutdown()
{
    mCurrentScene->shutdown();
	mCurrentScene->lateShutdown();
    mCurrentScene = nullptr;

}

void GameManager::finalShutdown() 
{
	//mAudioManager->unloadResources();
	mGraphicsManager->unloadResources();
	//OpenALUtil::shutdown();
	gl::shutdown();
}


// Update the game_loop function to limit FPS
//void GameManager::gameLoop()
//{
//    constexpr double fpsLimit = 1.0 / settings::max_fps;
//    double deltaTime = 0.0f;
//    double lastFrame = 0.0f;
//    double elapsedTime = 0.0f;
//    int frameCount = 0;
//
//    while (!glfwWindowShouldClose(mGameWindow))
//    {
//        OpenGlUtil::beginFrame();
//        const double currentFrame = glfwGetTime();
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//        elapsedTime += deltaTime;
//        frameCount++;
//
//        if (elapsedTime >= 1.0)
//        {
//            int fps = frameCount;
//            //std::cout << "FPS: " << fps << '\n';
//            frameCount = 0;
//            elapsedTime = 0.0;
//        }
//
//        glfwPollEvents();
//        update(static_cast<float>(deltaTime));
//        render();
//        OpenGlUtil::endFrame();
//        glfwSwapBuffers(mGameWindow);
//
//        // Limit FPS
//        const double frameTime = glfwGetTime() - currentFrame;
//        if (frameTime < fpsLimit)
//        {
//            std::this_thread::sleep_for(std::chrono::duration<double>(fpsLimit - frameTime));
//        }
//        if (mPendingScene)
//        {
//            shutdown();
//            mCurrentScene = std::move(mPendingScene);
//            init();
//
//        }
//    }
//}

void GameManager::gameLoop()
{
    const double targetFrameTime = 1.0 / settings::max_fps;
    auto previousTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(mGameWindow))
    {
        gl::beginFrame();
        auto loopStart = std::chrono::high_resolution_clock::now();
        double deltaTime = std::chrono::duration<double>(loopStart - previousTime).count();
        previousTime = loopStart;

        glfwPollEvents();
        update(static_cast<float>(deltaTime));
        render(static_cast<float>(deltaTime));
        gl::endFrame();
        // End frame
        glfwSwapBuffers(mGameWindow);

        // Measure loop time once, right after swap
        auto loopEnd = std::chrono::high_resolution_clock::now();
        double frameDuration = std::chrono::duration<double>(loopEnd - loopStart).count();
        double sleepTime = targetFrameTime - frameDuration;
        if (sleepTime > 0.0)
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));

        if (mPendingScene)
        {
            shutdown();
            mCurrentScene = std::move(mPendingScene);
            init();
        }
    }
}


GLFWwindow * GameManager::get_glfw_window()
{
    return mGameWindow;
}

void GameManager::set_glfw_window(GLFWwindow * window)
{
    mGameWindow = window;
}

