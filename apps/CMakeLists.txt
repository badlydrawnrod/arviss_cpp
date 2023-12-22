cmake_minimum_required(VERSION 3.14)

project(arviss_cppApps CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(arviss_cpp REQUIRED)
endif()

add_custom_target(run-apps)

function(add_app NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE arviss_cpp::arviss_cpp)
  target_compile_features("${NAME}" PRIVATE cxx_std_20)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-apps "run_${NAME}")
endfunction()

add_app(empty)
add_app(runner)

add_folders(Apps)
