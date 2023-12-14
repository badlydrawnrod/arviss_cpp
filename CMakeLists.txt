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

# # Here's where to find the app itself.
# add_subdirectory(apps)

# # Here's how the app knows what to include.
# target_include_directories(runner PUBLIC include)

add_library(arviss_cpp INTERFACE)
target_include_directories(
  arviss_cpp
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(arviss_cpp INTERFACE cxx_std_20)

install(TARGETS arviss_cpp
        EXPORT arvissTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(FILES include/arviss/** DESTINATION include/arviss)

install(EXPORT arvissTargets
        FILE arvissTargets.cmake
        NAMESPACE arviss_cpp::
        DESTINATION lib/cmake/arviss_cpp
        )
