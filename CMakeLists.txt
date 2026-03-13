cmake_minimum_required(VERSION 4.1)
project(project)

set(CMAKE_CXX_STANDARD 20)

add_executable(project src/main.cpp
        src/menu.cpp
        header/menu.h
        src/batch.cpp
        header/batch.h
)
