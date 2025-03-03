cmake_minimum_required(VERSION 3.22)

project(trajectorygen LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_TESTS OFF)

find_package(OpenGL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8 # Latest stable version
)
FetchContent_MakeAvailable(glfw)

# Add the include directories
add_executable(MotionPlanner mainpos.cpp)

target_include_directories(MotionPlanner PUBLIC 
                                               ${glfw_SOURCE_DIR}/include
                                                )

if(WIN32)
    target_compile_definitions(MotionPlanner PUBLIC glfw opengl32.lib)
else()
    target_link_libraries(MotionPlanner PUBLIC glfw OpenGL::GL GLU X11 Xxf86vm Xrandr Xinerama Xcursor dl pthread)
endif()

