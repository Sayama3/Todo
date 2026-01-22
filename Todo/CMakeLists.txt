cmake_minimum_required(VERSION 3.16)

set(TODO_SRC
		include/Todo/Todo.hpp
		include/Todo/Multithreading/Thread.hpp
		src/Multithreading/Thread.cpp
		include/Todo/Core/Macros.hpp
		include/Todo/Core/Logger.hpp
		src/Todo.cpp
		src/JobSystem/JobSystem.cpp
		include/Todo/JobSystem/JobSystem.hpp
		include/Todo/Core/Helper.hpp
		src/Core/Logger.cpp
		src/JobSystem/Job.cpp
		include/Todo/JobSystem/Job.hpp
		include/Todo/Multithreading/SpinlockMutex.hpp
		include/Todo/Multithreading/Lockguard.hpp
		src/Multithreading/SpinlockMutex.cpp
		include/Todo/Async/SharedFuture.hpp
		include/Todo/Async/Future.hpp
		src/Multithreading/ThreadsafeStack.cpp
		include/Todo/Multithreading/ThreadsafeStack.hpp
		src/Multithreading/Mutex.cpp
		include/Todo/Multithreading/Mutex.hpp
		src/Multithreading/ThreadsafeQueue.cpp
		include/Todo/Multithreading/ThreadsafeQueue.hpp
		include/Todo/Multithreading/TMutex.hpp
		include/Todo/Multithreading/TypePadder.hpp
		include/Todo/Multithreading/ThreadsafeMap.hpp
		include/Todo/Core/Concepts.hpp
		src/Multithreading/ThreadsafeList.cpp
		include/Todo/Multithreading/ThreadsafeList.hpp
		include/Todo/Core/Allocator.hpp
		include/Todo/Multithreading/LockFreeStack.h
		include/Todo/Multithreading/LockFreeQueue.hpp
        include/Todo/JobSystem/ThreadPool.hpp
		src/JobSystem/ThreadPool.cpp
		include/Todo/Async/Promise.hpp
		include/Todo/Async/PackagedTask.hpp
		include/Todo/Async/Result.hpp
        include/Todo/Async/ResultVoid.hpp
		include/Todo/Core/FunctionWrapper.hpp
		include/Todo/Async/SharedResult.hpp
		src/Async/Future.cpp
		src/Async/SharedFuture.cpp
		src/Async/Promise.cpp
		src/Async/Result.cpp
		include/Todo/Multithreading/WorkStealQueue.hpp
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

message(TODO_REMAINING_THREAD)
if(TODO_REMAINING_THREAD AND ${TODO_REMAINING_THREAD} GREATER 0)
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
		<concepts>
		<type_traits>
		<new>
		<variant>
		<optional>

		# Exception related stuff
		<exception>
		<stdexcept>

		# String & IO manipulation
		<filesystem>
		<string>
		<string_view>

		# Containers
		<array>
		<vector>
		<stack>
		<queue>

		# C-Types Helpers
		<cstdint>
		<cstring>
		<cmath>
)

add_library(Todo::Todo ALIAS Todo)

# Find pthread package
find_package(Threads REQUIRED)
target_link_libraries(Todo PUBLIC Threads::Threads)
