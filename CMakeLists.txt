cmake_minimum_required(VERSION 3.15)
project(test_cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

find_package(
    SFML 3
    REQUIRED
    COMPONENTS Graphics Window System
)

add_executable(test_cpp main.cpp)

target_link_libraries(test_cpp PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer -Wno-uninitialized")
