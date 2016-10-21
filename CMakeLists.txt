cmake_minimum_required(VERSION 3.2)
project(UnixSocketIPC)

set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 1)
set(PROJECT_PATCH_LEVEL 0)
set(PROJECT_VERSION ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_LEVEL})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DNDEBUG -std=c++11")

add_subdirectory(unix_socket_ipc)

add_executable(reader "source/reader.cpp")
target_link_libraries(reader UnixSocketIpcLib)

add_executable(writer "source/writer.cpp")
target_link_libraries(writer UnixSocketIpcLib)