# Specify the vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Minimum required version
cmake_minimum_required(VERSION 3.20)

# Project declaration
project(CamEngine LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable compile commands generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Display build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Set policies
if(POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif()

# Compiler options
if(MSVC)
    add_compile_options(/W4)
    
    # Debug Configuration
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Od /RTC1")
    
    # Release Configuration
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /O2 /Ob2 /Oi /Ot /Oy /GL")
else()
    add_compile_options(-Wall -Wextra -Wpedantic -g)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Function to add sanitizer flags
function(add_sanitizers target)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            target_compile_options(${target} PRIVATE -fsanitize=address -fsanitize=undefined)
            target_link_options(${target} PRIVATE -fsanitize=address -fsanitize=undefined)
        elseif(MSVC)
            target_compile_options(${target} PRIVATE /fsanitize=address)
            target_link_options(${target} PRIVATE /fsanitize=address)
        endif()
    endif()
endfunction()

# Find packages
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(directx-headers CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

# Manually list Engine source and header files
set(ENGINE_SOURCES
    sources/Engine/pch.cpp
    sources/Engine/Audio/AudioListener.cpp
    sources/Engine/Audio/AudioSource.cpp
    sources/Engine/Base/BaseScene.cpp
    sources/Engine/Base/ScriptBase.cpp
    sources/Engine/Collision/CollisionManifold.cpp
    sources/Engine/Collision/Octree.cpp
    sources/Engine/Collision/OctreeNode.cpp
    sources/Engine/Events/EventHandler.cpp
    sources/Engine/Graphics/Camera.cpp
    sources/Engine/Graphics/Font.cpp  # Corrected 'Grapics' to 'Graphics'
    sources/Engine/Graphics/GraphicsAPI.cpp
    sources/Engine/Graphics/Material.cpp
    sources/Engine/Graphics/Mesh.cpp
    sources/Engine/Graphics/Model.cpp
    sources/Engine/Graphics/Shader.cpp
    sources/Engine/Graphics/Texture.cpp
    sources/Engine/Graphics/VertexArray.cpp
    sources/Engine/Graphics/VertexBuffer.cpp
    sources/Engine/Graphics/IndexBuffer.cpp
    sources/Engine/Graphics/GraphicsContext.cpp
    sources/Engine/Graphics/Renderer.cpp
    sources/Engine/Graphics/Renderer2D.cpp
    sources/Engine/Managers/AudioManager.cpp
    sources/Engine/Managers/CollisionManager.cpp
    sources/Engine/Managers/CommandManager.cpp
    sources/Engine/Managers/GameManager.cpp
    sources/Engine/Managers/GraphicsManager.cpp
    sources/Engine/Managers/InputManager.cpp
    sources/Engine/Platform/OpenGL/OpenGLAPI.cpp
    sources/Engine/Platform/OpenGL/OpenGLIndexBuffer.cpp
    sources/Engine/Platform/OpenGL/OpenGLMaterial.cpp
    sources/Engine/Platform/OpenGL/OpenGLMesh.cpp
    sources/Engine/Platform/OpenGL/OpenGLModel.cpp
    sources/Engine/Platform/OpenGL/OpenGLShader.cpp
    sources/Engine/Platform/OpenGL/OpenGLTexture.cpp
    sources/Engine/Platform/OpenGL/OpenGLVertexArray.cpp
    sources/Engine/Platform/OpenGL/OpenGLVertexBuffer.cpp
    sources/Engine/Storage/Registry.cpp
    sources/Engine/Systems/SAudio.cpp
    sources/Engine/Systems/SCollision.cpp
    sources/Engine/Systems/SRender.cpp
    sources/Engine/Systems/SPhysics.cpp
    sources/Engine/Util/EngineUtil.cpp
    sources/Engine/Util/OpenALUtil.cpp
    sources/Engine/Util/OpenGLUtil.cpp
)

set(ENGINE_HEADERS
    sources/Engine/pch.hpp
    sources/Engine/Audio/AudioListener.hpp
    sources/Engine/Audio/AudioSource.hpp
    sources/Engine/Base/BaseScene.hpp
    sources/Engine/Base/ScriptBase.hpp
    sources/Engine/Collision/BoundingBox.hpp
    sources/Engine/Collision/CollisionManifold.hpp
    sources/Engine/Collision/Octree.hpp
    sources/Engine/Collision/OctreeNode.hpp
    sources/Engine/Events/Event.hpp
    sources/Engine/Events/EventHandler.hpp
    sources/Engine/Events/CollisionEvents.hpp
    sources/Engine/Events/CommandsEvents.hpp
    sources/Engine/Events/ComponentEvents.hpp
    sources/Engine/Events/InputEvents.hpp
    sources/Engine/Events/ScriptEvents.hpp
    sources/Engine/Graphics/Camera.hpp
    sources/Engine/Graphics/Font.hpp
    sources/Engine/Graphics/GraphicsAPI.hpp
    sources/Engine/Graphics/Material.hpp
    sources/Engine/Graphics/Mesh.hpp
    sources/Engine/Graphics/Model.hpp
    sources/Engine/Graphics/Shader.hpp
    sources/Engine/Graphics/Texture.hpp
    sources/Engine/Graphics/VertexArray.hpp
    sources/Engine/Graphics/VertexBuffer.hpp
    sources/Engine/Graphics/IndexBuffer.hpp
    sources/Engine/Graphics/GraphicsContext.hpp
    sources/Engine/Graphics/Renderer.hpp
    sources/Engine/Graphics/Renderer2D.hpp
    sources/Engine/Managers/AudioManager.hpp
    sources/Engine/Managers/CollisionManager.hpp
    sources/Engine/Managers/CommandManager.hpp
    sources/Engine/Managers/GameManager.hpp
    sources/Engine/Managers/GraphicsManager.hpp
    sources/Engine/Managers/InputManager.hpp
    sources/Engine/Platform/OpenGL/OpenGLAPI.hpp
    sources/Engine/Platform/OpenGL/OpenGLIndexBuffer.hpp
    sources/Engine/Platform/OpenGL/OpenGLMaterial.hpp
    sources/Engine/Platform/OpenGL/OpenGLMesh.hpp
    sources/Engine/Platform/OpenGL/OpenGLModel.hpp
    sources/Engine/Platform/OpenGL/OpenGLShader.hpp
    sources/Engine/Platform/OpenGL/OpenGLTexture.hpp
    sources/Engine/Platform/OpenGL/OpenGLVertexArray.hpp
    sources/Engine/Platform/OpenGL/OpenGLVertexBuffer.hpp
    sources/Engine/Storage/ISparseSet.hpp
    sources/Engine/Storage/Registry.hpp
    sources/Engine/Storage/Snapshot.hpp
    sources/Engine/Storage/SparseSet.hpp
    sources/Engine/Systems/SAudio.hpp
    sources/Engine/Systems/SCollision.hpp
    sources/Engine/Systems/SRender.hpp
    sources/Engine/Systems/SPhysics.hpp
    sources/Engine/Util/EngineUtil.hpp
    sources/Engine/Util/OpenALUtil.hpp
    sources/Engine/Util/OpenGLUtil.hpp
    sources/Engine/Util/GameSettings.hpp
    sources/Engine/Util/platform.hpp
    

    # Add additional Engine .hpp files here
)

# Manually list Game source and header files
set(GAME_SOURCES
    sources/User/main.cpp
    sources/User/Scenes/PlayScene.cpp
    sources/User/Scenes/MenuScene.cpp
    sources/User/Scenes/TestScene.cpp
    sources/User/Scripts/Factory.cpp
    sources/User/Scripts/Controllers/PlayerController.cpp
    sources/User/Scripts/Controllers/AsteroidController.cpp
    sources/User/Scripts/Controllers/BoundaryController.cpp
    sources/User/Scripts/Controllers/CameraController.cpp
    sources/User/Scripts/Controllers/EnemyShipController.cpp
    sources/User/Scripts/Controllers/HealthController.cpp
    sources/User/Scripts/Controllers/MainMenuController.cpp
    sources/User/Scripts/Controllers/SpinningLightController.cpp
    sources/User/Scripts/Controllers/SpinningModelController.cpp
    sources/User/Scripts/Controllers/TestSceneController.cpp
    sources/User/Scripts/Controllers/WaveController.cpp
    # Add additional Game .cpp files here
)

set(GAME_HEADERS
    sources/User/Factory.hpp
    sources/User/Scripts/Components/Asteroid.hpp
    sources/User/Scripts/Components/Damage.hpp
    sources/User/Scripts/Components/EnemyShip.hpp
    sources/User/Scripts/Components/Health.hpp
    sources/User/Scripts/Components/Player.hpp
    sources/User/Scripts/Components/Projectile.hpp
    sources/User/Scripts/Controllers/AsteroidController.hpp
    sources/User/Scripts/Controllers/BoundaryController.hpp
    sources/User/Scripts/Controllers/CameraController.hpp
    sources/User/Scripts/Controllers/EnemyShipController.hpp
    sources/User/Scripts/Controllers/HealthController.hpp
    sources/User/Scripts/Controllers/MainMenuController.hpp
    sources/User/Scripts/Controllers/PlayerController.hpp
    sources/User/Scripts/Controllers/SpinningLightController.hpp
    sources/User/Scripts/Controllers/SpinningModelController.hpp
    sources/User/Scripts/Controllers/TestSceneController.hpp
    sources/User/Scripts/Controllers/WaveController.hpp
    # Add additional Game .hpp files here
)

# Debugging messages to verify lists
message(STATUS "ENGINE_SOURCES: ${ENGINE_SOURCES}")
message(STATUS "ENGINE_HEADERS: ${ENGINE_HEADERS}")
message(STATUS "GAME_SOURCES: ${GAME_SOURCES}")
message(STATUS "GAME_HEADERS: ${GAME_HEADERS}")

# Create engine static library with source files only
add_library(engine_lib STATIC
    ${ENGINE_SOURCES}
)
target_precompile_headers(engine_lib PRIVATE "${CMAKE_SOURCE_DIR}/sources/Engine/pch.hpp")

# Include directories for engine library
target_include_directories(engine_lib PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/sources"
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include"
)

# Link libraries to engine library
target_link_libraries(engine_lib PUBLIC
    OpenGL::GL
    glfw
    OpenAL::OpenAL
    Freetype::Freetype
    glad::glad
    glm::glm
    Vulkan::Vulkan
    assimp::assimp
)

# Create game executable with source files only
add_executable(${PROJECT_NAME}
    ${GAME_SOURCES}
)
target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/sources/Engine/pch.hpp")

# Include directories for game executable
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/sources"
    "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include"
)

# Link engine library to game executable
target_link_libraries(${PROJECT_NAME} PRIVATE engine_lib)

# Add sanitizers (uncomment if needed)
# add_sanitizers(${PROJECT_NAME})

# Define ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

# Define SHADERS_PATH within assets
target_compile_definitions(${PROJECT_NAME} PUBLIC SHADERS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/")
