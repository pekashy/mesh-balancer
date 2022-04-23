set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(balancer)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES /usr/local/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(MAIN_SOURCE main.cpp)

file(MAKE_DIRECTORY cmake-build-debug)
find_package(proxygen REQUIRED)
find_package(gflags REQUIRED)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)

include_directories(include)
add_subdirectory(src)

add_executable(
        balancer_ex
        ${MAIN_SOURCE}
)
add_dependencies(BalancerLogic Common spdlog::spdlog)
add_dependencies(Server Common spdlog::spdlog)
add_dependencies(balancer_ex Server BalancerLogic Common)
target_link_libraries(BalancerLogic spdlog::spdlog)
target_link_libraries(Server spdlog::spdlog)

target_link_libraries(balancer_ex
        Server
        BalancerLogic
        Common
        proxygen::proxygen
        proxygen::proxygenhttpserver
        spdlog::spdlog
)

enable_testing()
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/tests TEST_SOURCES)
add_executable(
        BalancerTests
        ${TEST_SOURCES}
)
add_dependencies(BalancerTests Common)
target_link_libraries(BalancerTests gtest gtest_main pthread Server
        BalancerLogic
        Common
        proxygen::proxygen
        proxygen::proxygenhttpserver
        spdlog::spdlog
        )
