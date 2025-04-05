#include "pch.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "Engine/Util/Logging.hpp"
#include "Engine/Util/ErrorHandler.hpp"
#include "Engine/Util/VirtualFileSystem.hpp"
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

bool initializeVirtualFilesystem() {
	try {
		// Mount main directories to VFS
		auto& fs = vfs::FileSystem::instance();
		
		// Mount assets directory
		if (!fs.mount("/assets", "../assets")) {
			LOG_ERROR(logging::gEngineLogger, "Failed to mount assets directory");
			return false;
		}
		
		// Mount shaders directory
		if (!fs.mount("/shaders", "../src/Shaders")) {
			LOG_ERROR(logging::gEngineLogger, "Failed to mount shaders directory");
			return false;
		}
		
		// Mount logs directory
		std::filesystem::create_directory("logs");
		if (!fs.mount("/logs", "logs")) {
			LOG_ERROR(logging::gEngineLogger, "Failed to mount logs directory");
			return false;
		}
		
		LOG_INFO(logging::gEngineLogger, "Virtual filesystem initialized successfully");
		return true;
	}
	catch (const std::exception& e) {
		if (logging::gEngineLogger) {
			LOG_ERROR(logging::gEngineLogger, "Failed to initialize virtual filesystem: {}", e.what());
		} else {
			std::cerr << "Failed to initialize virtual filesystem: " << e.what() << std::endl;
		}
		return false;
	}
}

int main()
{
	try {
		// Initialize logging first
		logging::init(logging::LogLevel::Info);
		LOG_INFO(logging::gEngineLogger, "Starting application");
		
		// Setup crash handling
		error_handling::setupSignalHandlers();
		LOG_INFO(logging::gEngineLogger, "Signal handlers installed");
		
		// Initialize virtual filesystem
		if (!initializeVirtualFilesystem()) {
			LOG_CRITICAL(logging::gEngineLogger, "Failed to initialize virtual filesystem, aborting");
			return EXIT_FAILURE;
		}
		
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
	catch (const std::exception& e) {
		// Handle any uncaught exceptions
		if (logging::gEngineLogger) {
			LOG_CRITICAL(logging::gEngineLogger, "Fatal error in main: {}", e.what());
			error_handling::reportException(e);
			logging::shutdown();
		} else {
			std::cerr << "Fatal error: " << e.what() << std::endl;
		}
		return EXIT_FAILURE;
	}
	catch (...) {
		// Handle truly unknown exceptions
		if (logging::gEngineLogger) {
			LOG_CRITICAL(logging::gEngineLogger, "Unknown fatal error occurred");
			logging::shutdown();
		} else {
			std::cerr << "Unknown fatal error occurred" << std::endl;
		}
		return EXIT_FAILURE;
	}
}
