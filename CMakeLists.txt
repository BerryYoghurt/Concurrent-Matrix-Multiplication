cmake_minimum_required(VERSION 3.19)
project(MatMult C)

set(CMAKE_C_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(MatMult main.c parsing.c parsing.h)
target_link_libraries(MatMult PRIVATE Threads::Threads)