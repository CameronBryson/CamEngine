cmake_minimum_required(VERSION 3.5)
project(my_raylib_game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/include/glfw-3.4/include)
include_directories(${CMAKE_SOURCE_DIR}/sources)  # Include the sources directory

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

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

# Add glad source files
set(GLAD_SOURCES ${CMAKE_SOURCE_DIR}/include/glad/src/gl.c)

# Add project source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/sources/*.cpp")

# Declaring our executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${GLAD_SOURCES})

# Link libraries
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw)

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

enable_testing()

add_subdirectory(gtest)