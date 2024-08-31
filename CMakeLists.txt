cmake_minimum_required(VERSION 3.16)

project(
		Todo
		VERSION 1.0.0
		DESCRIPTION "A Job System written in c++20 to create and manage multithreaded application."
		LANGUAGES CXX
)
# Library options
option(TODO_USE_ALL_THREADS		"Whether to use all the threads available or leave one for the rest of the systems" ON)
option(TODO_REMAINING_THREAD	"The number of thread to leave to the system if possible" 1)
option(TODO_MIN_THREAD_COUNT	"The minimum number of thread to use in the Job System" 4)
# TODO: Add logging options

# Compiling options
option(TODO_STATIC				"Compile the Todo library as a static one. OFF is Shared." ON)
option(TODO_TESTS				"Add the tests to the project" ON)
option(TODO_EXAMPLES			"Add the examples executable to the project" ON)


add_subdirectory(Todo)

if(TODO_TESTS)
	include(CTest)
	add_subdirectory(Tests)
endif()

if(TODO_EXAMPLES)
	add_subdirectory(Examples)
endif()