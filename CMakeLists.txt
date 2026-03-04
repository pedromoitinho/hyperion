cmake_minimum_required(VERSION 3.10)
project(hyperion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(hyperion_lib src/hyperion.cpp)
target_include_directories(hyperion_lib PUBLIC include)

add_executable(hyperion_test tests/main.cpp)
target_link_libraries(hyperion_test hyperion_lib)

enable_testing()
add_test(NAME HyperionTests COMMAND hyperion_test)