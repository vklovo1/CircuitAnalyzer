cmake_minimum_required(VERSION 3.15)
project(CircuitAnalyzer)

set(CMAKE_CXX_STANDARD 14)

add_executable(CircuitAnalyzer main.cpp Circuit.cpp Circuit.h)