#include "GameManager.hpp"
#include <thread>
#include <chrono>
#include "Engine/Base/BaseScene.hpp"

#include <iostream>

#include "Engine/Util/OpenGLUtil.hpp"

GLFWwindow * GameManager::mGameWindow = nullptr;
std::unique_ptr<BaseScene> GameManager::mCurrentScene = nullptr;

void GameManager::init()
{
    OpenGlUtil::init();
    mCurrentScene->init();
}

void GameManager::update(float dt)
{
    mCurrentScene->update(dt);
    mCurrentScene->lateUpdate(dt);
}

void GameManager::render()
{
    mCurrentScene->render();
}

void GameManager::shutdown()
{
    mCurrentScene->shutdown();
    glfwDestroyWindow(mGameWindow);
    glfwTerminate();
}


// Update the game_loop function to limit FPS
void GameManager::gameLoop()
{
	constexpr double fpsLimit = 1.0 / settings::max_fps;
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
    double elapsedTime = 0.0f;
    int frameCount = 0;

    while( ! glfwWindowShouldClose(mGameWindow) )
    {
        const double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        elapsedTime += deltaTime;
        frameCount++;

        if( elapsedTime >= 1.0 )
        {
            int fps = frameCount;
            std::cout << "FPS: " << fps << '\n';
            frameCount = 0;
            elapsedTime = 0.0;
        }

        glfwPollEvents();
        update(static_cast<float>(deltaTime));
        render();
        glfwSwapBuffers(mGameWindow);

        // Limit FPS
        const double frameTime = glfwGetTime() - currentFrame;
        if( frameTime < fpsLimit )
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(fpsLimit - frameTime));
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