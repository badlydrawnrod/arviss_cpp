cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    arviss_cpp
    VERSION 0.1.0
    DESCRIPTION "Arviss cpp - A RISC-V Instruction Set Simulator written using C++ concepts"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(arviss_cpp_arviss_cpp INTERFACE)
add_library(arviss_cpp::arviss_cpp ALIAS arviss_cpp_arviss_cpp)

set_property(
    TARGET arviss_cpp_arviss_cpp PROPERTY
    EXPORT_NAME arviss_cpp
)

target_include_directories(
    arviss_cpp_arviss_cpp ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(arviss_cpp_arviss_cpp INTERFACE cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${arviss_cpp_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(apps)
  endif()
endif()

# ---- Developer mode ----

if(NOT arviss_cpp_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of arviss_cpp"
  )
endif()

include(cmake/dev-mode.cmake)
