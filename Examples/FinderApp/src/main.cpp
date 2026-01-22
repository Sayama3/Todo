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


#include <Todo/Todo.hpp>

#include <iostream>

#include "Todo/Core/Helper.hpp"


using Duration = std::chrono::duration<uint64_t, std::milli>;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock, Duration>;

static inline int rand(int seed)
{
	const uint64_t s = seed;
	constexpr uint64_t a = 1103515245;
	constexpr uint64_t c = 12345;
	constexpr uint64_t m = INT_MAX + 1ull;
	seed = static_cast<int>((a * s + c) % m);
	return seed;
}

int main(int argc, char* argv[])
{
	{
		Todo::Promise<uint64_t> promise{};
		// std::promise<uint64_t> promise{};
		auto future = promise.get_future();
		const TimePoint beg = Todo::ClockNow<Clock, Duration>();
		// const bool ready = future.is_ready();
		// const bool ready = future.wait_for(std::chrono::seconds(1)) != std::future_status::timeout;
		const bool ready = future.wait_until(std::chrono::steady_clock::now() + std::chrono::seconds(1)) != std::future_status::timeout;
		const TimePoint end = Todo::ClockNow<Clock, Duration>();

		std::cout << "Time: " << (end - beg).count() << "ms" << std::endl;
		if (ready)
		{
			std::cout << "is ready" << std::endl;
		}
		else
		{
			std::cout << "is not ready" << std::endl;
		}
	}

	{
		Todo::ThreadPool thread_pool;

		std::array<Todo::Future<int>, 6> futures;
		futures[0] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 1;});
		futures[1] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 2;});
		futures[2] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 3;});
		futures[3] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 4;});
		futures[4] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 5;});
		futures[5] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 6;});

		uint64_t done = 6;
		while (done)
		{
			for (auto& future : futures)
			{
				if (future.is_ready() && future.is_valid())
				{
					--done;
					std::cout << "Future " << future.get() << " is done" << std::endl;
				}
			}
		}
	}

	return 0;
}