cmake_minimum_required(VERSION 3.19)
project(MatMult C)

set(CMAKE_C_STANDARD 11)

add_executable(MatMult main.c parsing.c parsing.h)