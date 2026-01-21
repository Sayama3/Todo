//
// Created by Sayama on 31/08/2024.
//


#include <Todo/Todo.hpp>

#include <iostream>

#include "Todo/Core/Helper.hpp"


using Duration = std::chrono::duration<uint64_t, std::milli>;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock, Duration>;

static inline int rand(int seed)
{
	constexpr int a = 1103515245;
	constexpr int c = 12345;
	constexpr int m = 2147483648;//2e31;
	seed = (a * seed + c) % m;
	return seed;
}

int main(int argc, char* argv[])
{
	// TimePoint p = std::chrono::time_point_cast<TimePoint>(Clock::now());
	// TimePoint p = TimePoint(std::chrono::duration_cast<Duration>(Clock::now().time_since_epoch()));

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
		futures[1-1] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 1;});
		futures[2-1] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 2;});
		futures[3-1] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 3;});
		futures[4-1] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 4;});
		futures[5-1] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 5;});
		futures[6-1] = thread_pool.Submit([](){auto id = std::this_thread::get_id(); std::this_thread::sleep_for(std::chrono::seconds(rand(*(unsigned int*)&id) % 5u));return 6;});

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