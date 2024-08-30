cmake_minimum_required(VERSION 3.16)

set(TODO_SRC
    include/Todo/Thread.hpp
    src/Thread.cpp
)

if(TODO_STATIC)
    add_library(Todo STATIC)
else()
    add_library(Todo SHARED)
endif()

target_include_directories(Todo PUBLIC include)
target_include_directories(Todo PRIVATE src)

target_sources(Todo PRIVATE ${TODO_SRC})

set_property(TARGET Todo PROPERTY CXX_STANDARD 20)
set_property(TARGET Todo PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Todo PROPERTY CMAKE_CXX_STANDARD_REQUIRED OFF)

add_library(Todo::Todo ALIAS Todo)
