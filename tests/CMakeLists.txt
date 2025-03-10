cmake_minimum_required(VERSION 3.30.3)

project(scTests LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  SCClientTests.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  sc::sc
  spdlog::spdlog
  Threads::Threads
  GTest::gtest
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
