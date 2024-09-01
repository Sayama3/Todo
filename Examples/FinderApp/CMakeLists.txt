cmake_minimum_required(VERSION 3.16)

project(
		Fidner
		VERSION 1.0.0
		DESCRIPTION "An application made to search for a string in a text."
		LANGUAGES CXX
)

set(FINDER_SRC
		src/main.cpp
)

add_executable(Finder ${FINDER_SRC})

# C++ Properties of the library
set_target_properties(Finder PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
)

if(MSVC AND TODO_UNICODE)
	target_compile_options(Finder PUBLIC $<$<COMPILE_LANGUAGE:CXX>:/utf-8>)
endif()

target_precompile_headers(Finder REUSE_FROM Todo)
target_link_libraries(Finder PUBLIC Todo::Todo)