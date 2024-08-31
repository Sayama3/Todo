//
// Created by ianpo on 30/08/2024.
//

#include "Todo/Todo.hpp"
#include "Todo/Core/Macros.hpp"
#include "Todo/Core/Helper.hpp"
#include "Todo/Core/Logger.hpp"

namespace Todo
{
	static inline std::unique_ptr<JobSystem> s_JobSystem{nullptr};

	uint32_t GetAvailableThreadCount()
	{
		auto threadCount = std::thread::hardware_concurrency();
		return threadCount > 0 ? threadCount : TODO_MIN_THREAD_COUNT;
	}

	uint32_t GetOptimalThreadCount()
	{
		auto threadCount = GetAvailableThreadCount();
#ifndef TODO_USE_ALL_THREADS
		return std::max(TODO_MIN_THREAD_COUNT, threadCount - TODO_REMAINING_THREAD)
#else
		return threadCount;
#endif
	}

	void Todo::Initialize(uint32_t threadCount)
	{
		if(s_JobSystem) {
			TODO_ERROR("The Job System is already initialized. Replacing the old one with the new one.");
		}
		s_JobSystem = std::make_unique<JobSystem>(threadCount);
	}

	void Todo::Initialize(std::unique_ptr<JobSystem> jobsystem) {
		if(s_JobSystem) {
			TODO_ERROR("The Job System is already initialized. Replacing the old one with the new one.");
		}
		s_JobSystem = std::move(jobsystem);
	}

	void Destroy()
	{
		s_JobSystem.reset(nullptr);
	}
}