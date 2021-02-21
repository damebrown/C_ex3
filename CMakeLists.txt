cmake_minimum_required(VERSION 3.12)
project(ex3 C)

set(CMAKE_C_STANDARD 99)

add_executable(ex3 cmake-build-debug/calculator.c cmake-build-debug/heat_eqn.c cmake-build-debug/calculator.h cmake-build-debug/heat_eqn.h cmake-build-debug/reader.c)