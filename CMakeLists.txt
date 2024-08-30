cmake_minimum_required(VERSION 3.16)

project(
		TODO
		VERSION 1.0
		DESCRIPTION "A task/event scheduler/manager to create multithreaded application."
		LANGUAGES CXX
)

option(TODO_STATIC  "Compile the Todo library as a static one. OFF is Shared." ON)
option(TODO_TESTS   "Add the tests to the project" ON)
option(TODO_EXAMPLES    "Add the examples executable to the project" ON)

add_subdirectory(Todo)

if(TODO_TESTS)
	include(CTest)
	add_subdirectory(Tests)
endif()

if(TODO_EXAMPLES)
	add_subdirectory(Examples)
endif()