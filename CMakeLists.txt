cmake_minimum_required (VERSION 3.8)
project(BoolCalc)

set(CMAKE_CXX_STANDARD 14)

add_executable(BoolCalc
        boolcalc.cpp
        boolexpr.h
        boolexpr.cpp
        avltree.h
        formulanodes.h
        formulanodes.cpp
        error.h
        )