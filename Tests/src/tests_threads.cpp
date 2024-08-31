//
// Created by ianpo on 28/08/2024.
//

#include <gtest/gtest.h>
#include <Todo/Multithreading/Thread.hpp>
#include <cstdint>
#include <atomic>

static constexpr uint64_t TestThreadCount{4};
static constexpr uint64_t CounterTarget{1000};

TEST(TestThreads, HandleConstructionDestruction) {
  auto* threadptr = new Todo::Thread();
  EXPECT_TRUE(threadptr != nullptr) << "The Thread creation has failed.";
  delete threadptr;
  SUCCEED();
}


TEST(TestThreads, HandleJoin) {
	std::atomic<uint64_t> Counter;
	Todo::Thread t{[&Counter](){
		for (int i = 0; i < CounterTarget; ++i) {
			++Counter;
		}
	}};

	EXPECT_TRUE(t.Joinable());
	t.Join();
	EXPECT_FALSE(t.Joinable());

	EXPECT_EQ(Counter.load(), CounterTarget);
}

TEST(TestThreads, HandleAtomicIncrement) {
	std::atomic<uint64_t> Counter;

	std::vector<Todo::Thread> threads;
	threads.reserve(TestThreadCount);

	auto Incrementor = [&Counter](){
		for (int i = 0; i < CounterTarget; ++i) {
			++Counter;
		}
	};

	for (int i = 0; i < TestThreadCount; ++i) {
		threads.emplace_back(Incrementor);
	}

	for (auto& thread : threads)
	{
		thread.Join();
	}

	EXPECT_EQ(Counter.load(), CounterTarget * TestThreadCount);
}