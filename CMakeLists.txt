cmake_minimum_required(VERSION 3.5)
project(my_raylib_game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/sources)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif()

if(MSVC)
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/MDd /Od /Zi /fsanitize=address)
        add_link_options(/fsanitize=address)
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
    add_compile_options(-Wall -Wextra -Wpedantic -g -DCMAKE_EXPORT_COMPILE_COMMANDS=1)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-O0 -fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Add glad source files
set(GLAD_SOURCES ${CMAKE_SOURCE_DIR}/include/glad/src/gl.c)
set(STB_IMAGE_SOURCES ${CMAKE_SOURCE_DIR}/include/stb_image.h)

# Add project source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/sources/*.cpp")

# Declaring our executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${GLAD_SOURCES} ${STB_IMAGE_SOURCES})

if(MSVC AND CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(${PROJECT_NAME} PUBLIC clang_rt.asan_dynamic-x86_64.lib)
endif()
# Link libraries
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw)

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")

# Set ASan options for verbose output and log file
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(ENV{ASAN_OPTIONS} "log_path=${CMAKE_BINARY_DIR}/asan.log:verbosity=1")
endif()