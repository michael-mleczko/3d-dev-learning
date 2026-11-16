cmake_minimum_required(VERSION 3.21)

project(app3d VERSION 0.1.0 LANGUAGES C)

# C17 is the newest C standard that GCC, Clang, Apple Clang and MSVC all support.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Single-config generators (Makefiles, Ninja) default to an unoptimised build
# without debug info unless a build type is chosen.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

# GLFW: creates the window and the OpenGL context, delivers input.
# Downloaded and built from source at configure time, so there is nothing to
# install by hand on any platform.
include(FetchContent)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glfw)

# glad: loads the OpenGL function pointers. Generated for OpenGL 4.1 core and
# checked into third_party/glad (see README for how to regenerate).
add_library(glad STATIC third_party/glad/src/gl.c)
target_include_directories(glad SYSTEM PUBLIC third_party/glad/include)

# ---------------------------------------------------------------------------
# Application
# ---------------------------------------------------------------------------

add_executable(app3d
    src/main.c
)
target_link_libraries(app3d PRIVATE glfw glad)

if(MSVC)
    target_compile_options(app3d PRIVATE /W4)
    target_compile_definitions(app3d PRIVATE _CRT_SECURE_NO_WARNINGS)
    # Make app3d the project that F5 starts in Visual Studio.
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT app3d)
else()
    target_compile_options(app3d PRIVATE -Wall -Wextra -Wpedantic)
endif()
