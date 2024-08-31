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

if(TODO_DISABLE_LOG)
	target_compile_definitions(Todo PUBLIC TODO_DISABLE_LOG)
elseif (TODO_LOG)
	target_compile_definitions(Todo PUBLIC TODO_LOG)
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

add_library(Todo::Todo ALIAS Todo)
