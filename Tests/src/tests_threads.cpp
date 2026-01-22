//    Copyright 2026 https://github.com/Sayama3
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <Todo/Multithreading/Thread.hpp>

static constexpr uint64_t TestThreadCount{4};
static constexpr uint64_t CounterTarget{1000};

TEST(Threads, HandleConstructionDestruction) {
  auto* threadptr = new Todo::Thread();
  EXPECT_TRUE(threadptr != nullptr) << "The Thread creation has failed.";
  delete threadptr;
  SUCCEED();
}


TEST(Threads, HandleJoin) {
	std::atomic<uint64_t> Counter;
	Todo::Thread t{[&Counter](){
		for (int i = 0; i < CounterTarget; ++i) {
			++Counter;
		}
	}};

	EXPECT_TRUE(t.joinable());
	t.join();
	EXPECT_FALSE(t.joinable());

	EXPECT_EQ(Counter.load(), CounterTarget);
}

TEST(Threads, HandleAtomicIncrement) {
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
		thread.join();
	}

	EXPECT_EQ(Counter.load(), CounterTarget * TestThreadCount);
}