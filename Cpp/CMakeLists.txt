cmake_minimum_required(VERSION 3.10.0)

project(Flappy_Bird VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Flappy_Bird flapper.cpp)

find_package(SDL3 REQUIRED)
target_link_libraries(Flappy_Bird PRIVATE SDL3::SDL3)
find_package(SDL3_ttf REQUIRED)
target_link_libraries(Flappy_Bird PRIVATE SDL3_ttf::SDL3_ttf)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Score.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_SOURCE_DIR}/Geneva.ttf ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

include(CTest)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

