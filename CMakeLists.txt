cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME my_raylib_game)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
# Set CMake policy CMP0058 to NEW
if(POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif()

if(MSVC)
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/MDd /Od)
    else()
        add_compile_options(/MD
                /O2
                /Ob2
                /Oi
                /Ot
                /Oy
                /GL)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic -g)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-O0)
    else()
        add_compile_options(-O3)
    endif ()
endif()

# Enable shared library build
set(BUILD_SHARED_LIBS ON)

find_package(glfw3 3.3 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sources/*.cpp") # Define PROJECT_SOURCES as a list of all source files
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/sources/") # Define PROJECT_INCLUDE to be the path to the include directory of the project

# Declaring our executable
add_executable(${PROJECT_NAME}
        sources/Graphics/Camera.hpp
        sources/Graphics/Faces.hpp
        sources/Systems/SCamera.hpp
        sources/Graphics/Color.hpp
        sources/Engine/Collision.hpp
        sources/Graphics/Ray.hpp
        sources/Graphics/GraphicsUtil.hpp
        sources/Engine/EngineUtil.hpp
        sources/Shaders/ShaderProgram.hpp
        sources/platform.hpp
)
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw glad::glad)

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable

enable_testing()

add_subdirectory(gtest)