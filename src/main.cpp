#include "pch.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "Engine/Util/Logging.hpp"
#include "TestScene.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <wtypes.h>
#include <iostream>
#include <filesystem>
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


int main()
{
	// Initialize logging first
	logging::init(logging::LogLevel::Info);
	LOG_INFO(logging::gEngineLogger, "Starting application");
	
	
	// Initialize engine systems
	LOG_INFO(logging::gEngineLogger, "Initializing engine systems");
	GameManager::firstInit();
	
	// Load and initialize the test scene
	LOG_INFO(logging::gEngineLogger, "Creating test scene");
	GameManager::mCurrentScene = std::make_unique<TestScene>();
	
	// Initialize the scene
	LOG_INFO(logging::gEngineLogger, "Initializing scene");
	GameManager::init();
	
	// Run the game loop
	LOG_INFO(logging::gEngineLogger, "Entering main loop");
	GameManager::gameLoop();
	
	// Perform an orderly shutdown
	LOG_INFO(logging::gEngineLogger, "Shutting down");
	GameManager::shutdown();
	GameManager::finalShutdown();
	
	// Clean up systems
	LOG_INFO(logging::gEngineLogger, "Shutting down logging system");
	logging::shutdown();
	
	return EXIT_SUCCESS;
}
