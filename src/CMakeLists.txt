cmake_minimum_required(VERSION 3.16.3)
project(balancer)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

add_library(Common Logger.cpp Config.cpp)
target_link_libraries(Common spdlog::spdlog yaml-cpp)

add_subdirectory(Server)
add_subdirectory(Balancer)
