set(SOURCE_FILES main.cpp)

add_executable(async_app ${SOURCE_FILES})

target_link_libraries(async_app async_plat)
