cmake_minimum_required(VERSION 3.19)
project(MDSDVRP)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        main.h
        BinaryHeap.cpp
        BinaryHeap.h
        Example.cpp
        Globals.h
        Graph.cpp
        Graph.h
        Matching.cpp
        Matching.h)

add_executable(MDSDVRP ${SOURCE_FILES})


