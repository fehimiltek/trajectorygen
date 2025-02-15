cmake_minimum_required(VERSION 3.22)

project(generator LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_TESTS OFF)

# Set the project name
# project(generator)


find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

set(CMAKE_AUTOMOC ON) # ✅ This ensures `moc` runs automatically
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    Task
    GIT_REPOSITORY https://github.com/MILTEKSAN/Task.git
    GIT_TAG main
)
FetchContent_MakeAvailable(Task)

# Add the include directories
add_executable(QtMotionPlanner mainpos.cpp MainWindow.cpp)

target_include_directories(QtMotionPlanner PUBLIC ${Task_SOURCE_DIR} Qt6::Widgets Qt6::Charts)
target_link_libraries(QtMotionPlanner PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt6::OpenGLWidgets
    Qt6::Charts
    Task
)

add_executable(main main.cpp)
