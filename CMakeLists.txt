project(balancer)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES /usr/local/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})
file(MAKE_DIRECTORY cmake-build-debug)

find_package(proxygen REQUIRED)
find_package(gflags REQUIRED)

include_directories(include)
add_subdirectory(src)

add_executable(
    balancer
    main.cpp
)

target_link_libraries(balancer PUBLIC
        Server
    proxygen::proxygen
    proxygen::proxygenhttpserver
)