//
// Created by ianpo on 01/09/2024.
//

#include <gtest/gtest.h>
#include <Todo/Todo.hpp>
#include "Profiler.hpp"

#define TODO_TEST_THREAD_COUNT 32
#define TODO_TEST_ELEMENT_COUNT 1000

TEST(ThreadsafeStack, EnsurePush)
{
	// Simple flag to ensure all threads start at the same time.
	std::atomic_flag flag;
	while (flag.test_and_set(std::memory_order_release));

	// Creating the stack
	Todo::ThreadsafeStack<int> m_Stack;

	// Creating all the threads
	std::array<Todo::Thread, TODO_TEST_THREAD_COUNT> threads;


	{
		SCOPED_PROFILING("Create Threads");
		for (int i = 0; i < TODO_TEST_THREAD_COUNT; ++i) {
			threads[i] = [&m_Stack, &flag]() {
				while (flag.test(std::memory_order_acquire));
				for (int j = 0; j < TODO_TEST_ELEMENT_COUNT; ++j) {
					m_Stack.push(j);
				}
			};
		}
	}
	{
		SCOPED_PROFILING("Push Data");
		// Authorising all the threads to start.
		flag.clear(std::memory_order_release);

		// Joining them all.
		for (auto &t: threads) {
			t.Join();
		}
	}
	uint64_t stackCount;
	{
		SCOPED_PROFILING("Count Elements");
		stackCount = m_Stack.count();
	}
	// Asserting we have THREAD_COUNT * ELEMENT_COUNT elements in the stack.
	EXPECT_EQ(stackCount, TODO_TEST_THREAD_COUNT * TODO_TEST_ELEMENT_COUNT);
}

TEST(SafeQueue, EnsurePush)
{
	// Simple flag to ensure all threads start at the same time.
	std::atomic_flag flag;
	while (flag.test_and_set(std::memory_order_release));

	// Creating the stack
	Todo::SafeQueue<int> m_SafeQueue {};
	std::array<Todo::Thread, TODO_TEST_THREAD_COUNT> threads;

	{
		SCOPED_PROFILING("Create Threads");
		// Creating all the threads
		for (int i = 0; i < TODO_TEST_THREAD_COUNT; ++i) {
			threads[i] = [&m_SafeQueue, &flag]() {
				while (flag.test(std::memory_order_acquire));
				for (int j = 0; j < TODO_TEST_ELEMENT_COUNT; ++j) {
					m_SafeQueue.push(j);
				}
			};
		}
	}

	{
		SCOPED_PROFILING("Push Data");
		// Authorising all the threads to start.
		flag.clear(std::memory_order_release);

		// Joining them all.
		for (auto &t: threads) {
			t.Join();
		}
	}

	uint64_t queueCount;
	{
		SCOPED_PROFILING("Count Elements");
		queueCount = m_SafeQueue.calculate_count();
	}

	// Asserting we have THREAD_COUNT * ELEMENT_COUNT elements in the stack.
	EXPECT_EQ(queueCount, m_SafeQueue.count());
	EXPECT_EQ(queueCount, TODO_TEST_THREAD_COUNT * TODO_TEST_ELEMENT_COUNT);


	{
		SCOPED_PROFILING("Clear Queue");
		m_SafeQueue.clear();
	}

	{
		SCOPED_PROFILING("Count Elements");
		queueCount = m_SafeQueue.calculate_count();
	}

	// Asserting we have THREAD_COUNT * ELEMENT_COUNT elements in the stack.
	EXPECT_EQ(queueCount, 0);
	EXPECT_EQ(queueCount, m_SafeQueue.count());
}
