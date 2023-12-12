# I am not a CMake expert, so cargo-culting from various sources including...
# - things I've done before
# - https://gitlab.com/CLIUtils/modern-cmake/-/blob/master/examples/extended-project/CMakeLists.txt

cmake_minimum_required(VERSION 3.22)

project(
    arviss_cpp
    VERSION 0.1
    DESCRIPTION "Arviss cpp - A RISC-V Instruction Set Simulator written using C++ concepts"
    LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # C++20, no backsliding, no extensions.
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# I don't need this yet, but I probably will.
include(FetchContent)

# Here's where to find the app itself.
add_subdirectory(apps)

# Here's how the app knows what to include.
target_include_directories(runner PUBLIC include)
