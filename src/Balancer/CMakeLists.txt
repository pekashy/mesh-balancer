cmake_minimum_required(VERSION 3.16)
project(balancer)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} DIR_SOURCES)
add_library(BalancerLogic ${DIR_SOURCES})
