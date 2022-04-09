cmake_minimum_required(VERSION 3.16.3)
project(balancer)
find_package(spdlog REQUIRED)

add_library(Common Logger.cpp)
target_link_libraries(Common spdlog::spdlog)

add_subdirectory(Server)
add_subdirectory(Balancer)