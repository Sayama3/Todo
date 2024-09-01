//
// Created by ianpo on 01/09/2024.
//

#include <gtest/gtest.h>
#include <Todo/Todo.hpp>

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
	for (int i = 0; i < TODO_TEST_THREAD_COUNT; ++i) {
		threads[i] = [&m_Stack,&flag](){
			while (flag.test(std::memory_order_acquire));
			for (int j = 0; j < TODO_TEST_ELEMENT_COUNT; ++j) {
				m_Stack.push(j);
			}
		};
	}
	// Authorising all the threads to start.
	flag.clear(std::memory_order_release);

	// Joining them all.
	for(auto& t : threads)
	{
		t.Join();
	}

	// Asserting we have THREAD_COUNT * ELEMENT_COUNT elements in the stack.
	EXPECT_EQ(m_Stack.count(), TODO_TEST_THREAD_COUNT * TODO_TEST_ELEMENT_COUNT);
}

TEST(ThreadsafeQueue, EnsurePush)
{
	// Simple flag to ensure all threads start at the same time.
	std::atomic_flag flag;
	while (flag.test_and_set(std::memory_order_release));

	// Creating the stack
	Todo::ThreadsafeQueue<int> m_Stack;

	// Creating all the threads
	std::array<Todo::Thread, TODO_TEST_THREAD_COUNT> threads;
	for (int i = 0; i < TODO_TEST_THREAD_COUNT; ++i) {
		threads[i] = [&m_Stack,&flag](){
			while (flag.test(std::memory_order_acquire));
			for (int j = 0; j < TODO_TEST_ELEMENT_COUNT; ++j) {
				m_Stack.push(j);
			}
		};
	}
	// Authorising all the threads to start.
	flag.clear(std::memory_order_release);

	// Joining them all.
	for(auto& t : threads)
	{
		t.Join();
	}

	// Asserting we have THREAD_COUNT * ELEMENT_COUNT elements in the stack.
	EXPECT_EQ(m_Stack.count(), TODO_TEST_THREAD_COUNT * TODO_TEST_ELEMENT_COUNT);
}
