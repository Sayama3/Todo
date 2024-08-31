cmake_minimum_required(VERSION 3.16)

# Fetch googletest

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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
)

# C++ Properties of the library
set_target_properties(test_todo PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
)

target_precompile_headers(test_todo REUSE_FROM Todo)
target_link_libraries(test_todo PUBLIC
  Todo
  GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(test_todo)