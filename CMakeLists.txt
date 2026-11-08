cmake_minimum_required(VERSION 3.16)
project(star-tracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# ---- Dependencies: system first, download as fallback ----
include(FetchContent)

find_package(Eigen3 3.3 QUIET NO_MODULE)
if(NOT Eigen3_FOUND)
  message(STATUS "Eigen not found on system, downloading")
  FetchContent_Declare(eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0)
  set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(eigen)
endif()

find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
  message(STATUS "Catch2 not found on system, downloading")
  FetchContent_Declare(catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2)
  FetchContent_MakeAvailable(catch2)
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

# ---- Warnings ----
add_library(st_warnings INTERFACE)
if(MSVC)
  target_compile_options(st_warnings INTERFACE /W4)
else()
  target_compile_options(st_warnings INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

# ---- Library: everything in src/ ----
# Add new .cpp files here as you reach each phase.
add_library(startracker
  src/frames.cpp
  src/catalog.cpp
)
target_include_directories(startracker PUBLIC include)
target_link_libraries(startracker PUBLIC Eigen3::Eigen PRIVATE st_warnings)

# ---- Apps: one executable per file in apps/ ----
function(st_add_app name)
  add_executable(${name} apps/${name}.cpp)
  target_link_libraries(${name} PRIVATE startracker st_warnings)
endfunction()

st_add_app(check_catalog)
# Later phases:
# st_add_app(run_simulator_check)
# st_add_app(experiment_attitude)
# st_add_app(experiment_identify)
# st_add_app(solve_image)

# ---- Tests ----
enable_testing()
add_executable(star_tests
  tests/test_frames.cpp
  tests/test_catalog.cpp
)
target_link_libraries(star_tests PRIVATE startracker Catch2::Catch2WithMain st_warnings)
target_compile_definitions(star_tests PRIVATE
  ST_CATALOG_PATH="${CMAKE_SOURCE_DIR}/data/catalog.csv")
include(CTest)
include(Catch)
catch_discover_tests(star_tests)
