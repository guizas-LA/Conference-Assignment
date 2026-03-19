cmake_minimum_required(VERSION 4.1)
project(project)

set(CMAKE_CXX_STANDARD 20)

add_executable(project src/main.cpp
        src/menu.cpp
        header/menu.h
        src/batch.cpp
        header/batch.h
        header/structures.h
        header/parser.h
        src/parser.cpp
        src/show.cpp
        header/show.h
        header/assignment.h
        src/assignment.cpp
        header/maxflow.h
        src/maxflow.cpp
)
