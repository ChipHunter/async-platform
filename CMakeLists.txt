cmake_minimum_required(VERSION 3.2)
project(async_platform_lib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(INCLUDE_JUST_FOR_NOW ${PROJECT_SOURCE_DIR}/src)

include_directories(${INCLUDE_DIR})
include_directories(${INCLUDE_JUST_FOR_NOW})

add_subdirectory(src)
add_subdirectory(app)
#add_subdirectory(test)


