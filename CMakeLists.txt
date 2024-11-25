cmake_minimum_required(VERSION 3.5)
project(my_raylib_game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Message to display build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Set policies
if(POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif()

# Compiler options
if(MSVC)
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(/MDd /Od)
    else()
        add_compile_options(/MD /O2 /Ob2 /Oi /Ot /Oy /GL)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic -g -DCMAKE_EXPORT_COMPILE_COMMANDS=1)
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

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/glad/include)
include_directories(${CMAKE_SOURCE_DIR}/sources)

# Find packages
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Freetype REQUIRED)

# Add GLAD and STB source files
set(GLAD_SOURCES ${CMAKE_SOURCE_DIR}/include/glad/src/gl.c)
set(STB_IMAGE_SOURCES ${CMAKE_SOURCE_DIR}/include/stb_image.h)
set(STB_VORBIS_SOURCES ${CMAKE_SOURCE_DIR}/include/stb_vorbis.c)

# Collect engine source files
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/sources/Engine/*.cpp")

# Collect game source files
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/sources/User/*.cpp")

# Create engine static library
add_library(engine_lib STATIC ${ENGINE_SOURCES} ${GLAD_SOURCES} ${STB_IMAGE_SOURCES} ${STB_VORBIS_SOURCES})

# Include directories for engine library
target_include_directories(engine_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glad/include
    ${CMAKE_SOURCE_DIR}/sources
)

# Link libraries to engine library
target_link_libraries(engine_lib PUBLIC OpenGL::GL glfw OpenAL::OpenAL Freetype::Freetype)

# Create game executable
add_executable(${PROJECT_NAME} ${GAME_SOURCES})

# Include directories for game executable
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glad/include
    ${CMAKE_SOURCE_DIR}/sources
)

# Link engine library to game executable
target_link_libraries(${PROJECT_NAME} PRIVATE engine_lib)

# Add sanitizers (uncomment if needed)
# add_sanitizers(${PROJECT_NAME})

# Define ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
