cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    arviss_cpp
    VERSION 0.1.0
    DESCRIPTION "Arviss cpp - A RISC-V Instruction Set Simulator written using C++ concepts"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    arviss_cpp_arviss_cpp
    source/arviss_cpp.cpp
)
add_library(arviss_cpp::arviss_cpp ALIAS arviss_cpp_arviss_cpp)

include(GenerateExportHeader)
generate_export_header(
    arviss_cpp_arviss_cpp
    BASE_NAME arviss_cpp
    EXPORT_FILE_NAME export/arviss_cpp/arviss_cpp_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(arviss_cpp_arviss_cpp PUBLIC ARVISS_CPP_STATIC_DEFINE)
endif()

set_target_properties(
    arviss_cpp_arviss_cpp PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME arviss_cpp
    OUTPUT_NAME arviss_cpp
)

target_include_directories(
    arviss_cpp_arviss_cpp ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    arviss_cpp_arviss_cpp SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(arviss_cpp_arviss_cpp PUBLIC cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${arviss_cpp_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
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
