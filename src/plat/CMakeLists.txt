cmake_minimum_required(VERSION 3.2)
project(async_platform C CXX)

set(SOURCE_FILES
    test_timer.cpp
)

add_library(async_plat SHARED STATIC ${SOURCE_FILES})
find_package(Threads REQUIRED)
target_link_libraries(async_plat PRIVATE Threads::Threads) 
install(TARGETS async_plat DESTINATION ${DIVISIBLE_INSTALL_LIB_DIR})
#install(FILES division.h DESTINATION ${DIVISIBLE_INSTALL_INCLUDE_DIR})
