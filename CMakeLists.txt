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

file(GLOB ST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(startracker ${ST_SOURCES})

target_include_directories(startracker 
  PUBLIC ${CMAKE_SOURCE_DIR}/include
  PRIVATE ${CMAKE_SOURCE_DIR}/third_party/stb)
target_link_libraries(startracker PUBLIC Eigen3::Eigen PRIVATE st_warnings)
target_compile_features(startracker PUBLIC cxx_std_17)
target_compile_definitions(startracker PUBLIC ST_CATALOG_PATH="${CMAKE_SOURCE_DIR}/data/catalog.csv")

# ---- Apps: one executable per file in apps/ ----
file(GLOB ST_APP_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/apps/*.cpp)
foreach(app_src ${ST_APP_SOURCES})
  get_filename_component(app_name ${app_src} NAME_WE)
  add_executable(${app_name} ${app_src})
  target_link_libraries(${app_name} PRIVATE startracker st_warnings)
endforeach()

# ---- Tests ----
enable_testing()
file(GLOB ST_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
add_executable(star_tests ${ST_TEST_SOURCES})
target_link_libraries(star_tests PRIVATE startracker Catch2::Catch2WithMain st_warnings)
include(CTest)
include(Catch)
catch_discover_tests(star_tests)
