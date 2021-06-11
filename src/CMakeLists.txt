cmake_minimum_required(VERSION 3.2)
project(divider)

add_subdirectory(plat)
set(SOURCE_FILES main.cpp)

add_executable(divider ${SOURCE_FILES})
target_link_libraries(divider async_plat)
install(TARGETS divider DESTINATION ${DIVISIBLE_INSTALL_BIN_DIR})
