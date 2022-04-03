set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(balancer)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES /usr/local/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(MAIN_SOURCE main.cpp)

file(MAKE_DIRECTORY cmake-build-debug)
find_package(proxygen REQUIRED)
find_package(gflags REQUIRED)
find_package(glog REQUIRED)

include_directories(include)
add_subdirectory(src)

add_executable(
        balancer_ex
        ${MAIN_SOURCE}
)

add_dependencies(balancer_ex Server BalancerLogic)

target_link_libraries(balancer_ex
        Server
        BalancerLogic
        proxygen::proxygen
        proxygen::proxygenhttpserver
        glog::glog
)