cmake_minimum_required(VERSION 3.16)

# Fetch googletest

include(FetchContent)
FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG v1.15.2
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add the tests

enable_testing()

add_executable(
		test_todo
		src/tests_basics.cpp
		src/tests_threads.cpp
		src/tests_lockbased_datastructs.cpp
		src/Profiler.cpp
		include/Profiler.hpp
		include/Logger.hpp
)
target_include_directories(test_todo PRIVATE include)

# C++ Properties of the library
set_target_properties(test_todo PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
)

if(MSVC AND TODO_UNICODE)
	target_compile_options(Todo PUBLIC $<$<COMPILE_LANGUAGE:CXX>:/utf-8>)
endif()

target_precompile_headers(test_todo REUSE_FROM Todo)
target_link_libraries(test_todo PUBLIC
		Todo
		GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_todo)