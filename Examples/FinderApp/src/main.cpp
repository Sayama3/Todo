//
// Created by Sayama on 31/08/2024.
//


#include <Todo/Todo.hpp>

#include <iostream>



using Duration = std::chrono::duration<uint64_t, std::nano>;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock, Duration>;

int main(int argc, char* argv[])
{
	{
		Todo::Promise<uint64_t> promise{};
		// std::promise<uint64_t> promise{};
		auto future = promise.get_future();
		const TimePoint beg = Clock::now();
		const bool ready = future.is_ready();
		// const bool ready = future.wait_for(std::chrono::seconds{0}) != std::future_status::timeout;
		const TimePoint end = Clock::now();

		std::cout << "Time: " << (end - beg).count() << "ns" << std::endl;
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

		auto f = thread_pool.Submit([](){return 1;});
	}

	return 0;
}