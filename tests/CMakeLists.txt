cmake_minimum_required(VERSION 3.30.3)

project(scTests LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  scTests.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  sc::sc
  gtest::gtest
)

target_link_libraries(sc PRIVATE spdlog::spdlog)


include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

