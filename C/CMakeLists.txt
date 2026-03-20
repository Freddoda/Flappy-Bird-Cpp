cmake_minimum_required(VERSION 3.10.0)

project(Flappy_Bird VERSION 0.1.0 LANGUAGES C)

add_executable(Flappy_Bird flappy.c)

find_package(SDL3 REQUIRED)
target_link_libraries(Flappy_Bird PRIVATE SDL3::SDL3)
find_package(SDL3_ttf REQUIRED)
target_link_libraries(Flappy_Bird PRIVATE SDL3_ttf::SDL3_ttf)

include(CTest)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)