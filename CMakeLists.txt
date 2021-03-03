cmake_minimum_required(VERSION 3.10)
project(Starsurge)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find our packages
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Our subdirectories
add_subdirectory(src)
add_subdirectory(tests)
