cmake_minimum_required(VERSION 3.14)

project(arviss_cppTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(arviss_cpp REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(arviss_cpp_test source/arviss_cpp_test.cpp)
target_link_libraries(arviss_cpp_test PRIVATE arviss_cpp::arviss_cpp)
target_compile_features(arviss_cpp_test PRIVATE cxx_std_20)

add_test(NAME arviss_cpp_test COMMAND arviss_cpp_test)

# ---- End-of-file commands ----

add_folders(Test)
