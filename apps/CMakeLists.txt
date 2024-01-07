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

add_app(bencher)
add_app(re-encoder)
add_app(runner)
add_app(windows_memory)

add_folders(Apps)

if(NOT DEFINED ASMJIT_STATIC)
  set(ASMJIT_STATIC TRUE)
endif()

include(FetchContent)

FetchContent_Declare(
  asmjit
  GIT_REPOSITORY https://github.com/asmjit/asmjit
  GIT_TAG        master
)

FetchContent_MakeAvailable(asmjit)

add_app(use_asmjit)
target_link_libraries(use_asmjit PRIVATE AsmJit::AsmJit)
