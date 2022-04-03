cmake_minimum_required(VERSION 3.16)
project(balancer)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} DIR_SOURCES)
add_library(Server ${DIR_SOURCES})

find_package(glog REQUIRED)
target_link_libraries(Server PUBLIC glog::glog)
