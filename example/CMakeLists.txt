cmake_minimum_required(VERSION 3.25)

project(scExample LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)


target_link_libraries(${PROJECT_NAME} PRIVATE sc::sc)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
