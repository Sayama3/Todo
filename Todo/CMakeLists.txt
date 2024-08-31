cmake_minimum_required(VERSION 3.16)

set(TODO_SRC
		include/Todo/Todo.hpp
		include/Todo/Multithreading/Thread.hpp
		src/Thread.cpp
		include/Todo/Core/Macros.hpp
		include/Todo/Core/Logger.hpp
		src/Todo.cpp
		src/JobSystem.cpp
		include/Todo/JobSystem/JobSystem.hpp
		include/Todo/Core/Helper.hpp
		src/Logger.cpp
)

if(TODO_STATIC)
	add_library(Todo STATIC ${TODO_SRC})
else()
	add_library(Todo SHARED ${TODO_SRC})
endif()

target_include_directories(Todo PUBLIC include)
target_include_directories(Todo PRIVATE src)

# C++ Properties of the library
set_target_properties(Todo PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
)

if(MSVC AND TODO_UNICODE)
	target_compile_options(Todo PUBLIC $<$<COMPILE_LANGUAGE:CXX>:/utf-8>)
endif()

# Custom properties of the library
if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	target_compile_definitions(Todo PUBLIC TODO_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
	target_compile_definitions(Todo PUBLIC TODO_RELEASE)
elseif(CMAKE_BUILD_TYPE MATCHES "[Dd][Ii][Ss][Tt]")
	target_compile_definitions(Todo PUBLIC TODO_DIST)
else()
	message(WARNING, "The build type ${CMAKE_BUILD_TYPE} is unknown. Falling back to Release mode.")
	target_compile_definitions(Todo PUBLIC TODO_RELEASE)
endif()

if(TODO_USE_ALL_THREADS)
	target_compile_definitions(Todo PUBLIC TODO_USE_ALL_THREADS)
else()
	if(TODO_MIN_THREAD_COUNT)
		target_compile_definitions(Todo PUBLIC TODO_MIN_THREAD_COUNT=${TODO_MIN_THREAD_COUNT})
	endif()
endif()

if(TODO_REMAINING_THREAD)
	target_compile_definitions(Todo PUBLIC TODO_REMAINING_THREAD=${TODO_REMAINING_THREAD})
endif()

target_precompile_headers(Todo PUBLIC
		# Multithreading headers
		<thread>
		<atomic>
		<future>
		<condition_variable>
		<mutex>
		<shared_mutex>
		<latch>
		<barrier>

		# Utility stuff
		<functional>
		<utility>
		<algorithm>
		<memory>
		<source_location>

		# String & IO manipulation
		<filesystem>
		<string>
		<string_view>

		# Containers
		<optional>
		<tuple>
		<variant>
		<array>
		<any>
		<vector>
		<map>
		<unordered_map>
		<set>
		<unordered_set>

		# C-Types Helpers
		<cstdint>
		<cstring>
		<cmath>
)

if (TODO_LOG)
	include(FetchContent)
	FetchContent_Declare(
			fmt
			GIT_REPOSITORY https://github.com/fmtlib/fmt.git
			GIT_TAG        11.0.2
	)

	# Disabling all options as I only need the basics. (and compiling portability).
	set(FMT_DOC				OFF) # "Generate the doc target."
	set(FMT_INSTALL			OFF) # "Generate the install target."
	set(FMT_TEST			OFF) # "Generate the test target."
	set(FMT_FUZZ			OFF) # "Generate the fuzz target."
	set(FMT_CUDA_TEST		OFF) # "Generate the cuda-test target."
	set(FMT_OS				OFF) # "Include OS-specific APIs."
	set(FMT_MODULE			OFF) # "Build a module instead of a traditional library."
	set(FMT_SYSTEM_HEADERS	OFF) # "Expose headers with marking them as system."
	set(FMT_UNICODE			${TODO_UNICODE}) # "Enable Unicode support."

	FetchContent_MakeAvailable(fmt)
	target_link_libraries(Todo PRIVATE fmt::fmt)
	target_compile_definitions(Todo PRIVATE TODO_LOG)
endif()

add_library(Todo::Todo ALIAS Todo)
