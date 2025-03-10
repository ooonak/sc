cmake_minimum_required(VERSION 3.30.3)

project(
  sc
  VERSION 0.0.1
  DESCRIPTION "Simple Communicator (SC)"
  LANGUAGES CXX)

option(TESTS "Build tests" ON)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  include(GNUInstallDirs)
  include(CTest)
endif()

find_package(Threads REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(example)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
  AND TESTS)
  find_package(GTest REQUIRED)
  add_subdirectory(tests)
endif()
