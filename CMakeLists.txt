cmake_minimum_required(VERSION 3.13)
project(serverThreadPool)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(serverThreadPool
        include/epoll.h
        include/httpData.h
        include/macros.h
        include/SafeQueue.h
        include/server.h
        include/threadPoolBar.h
        include/Util.h
        src/epoll.cpp
        src/httpData.cpp
        src/Util.cpp
        main.cpp)
