cmake_minimum_required(VERSION 3.0)
project(my_raylib_game CXX)
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fsanitize=address -fsanitize=leak -fsanitize=undefined")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fsanitize=address -fsanitize=leak -fsanitize=undefined -DENABLE_ASAN:BOOL=ON")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -DENABLE_ASAN:BOOL=ON")


# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sources/*.cpp") # Define PROJECT_SOURCES as a list of all source files
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/sources/") # Define PROJECT_INCLUDE to be the path to the include directory of the project


# Declaring our executable
add_executable(${PROJECT_NAME}
        sources/CRender.hpp
        sources/CRigidBody.hpp
        sources/CTransform.hpp
        sources/CSprite.hpp
        sources/CStaticBody.hpp
        sources/CKineticBody.hpp
        sources/GameSettings.hpp
        sources/Delegate.hpp
        sources/ISparseSet.hpp
        sources/SparseSet.hpp
        sources/CPlayer.hpp
        sources/ThreadPool.hpp
        sources/Timer.hpp
        sources/Stats.hpp
        sources/Components.hpp
        sources/Systems.hpp
        sources/Stats.cpp
        sources/SGravity.cpp
        sources/SGravity.hpp
        sources/Factory.cpp
        sources/Factory.hpp
        sources/CGravity.hpp

)
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable

