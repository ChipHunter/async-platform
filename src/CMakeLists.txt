set(SOURCE_FILES
    asio_timer.h
    asio_timer.cpp
    asio_unix_socket.h
    asio_unix_socket.cpp
    async_platform.cpp
    async_platform.h
)

add_library(async_plat SHARED STATIC ${SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(async_plat PRIVATE Threads::Threads) 
