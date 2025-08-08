//
// Created by ianpo on 30/08/2024.
//

#include "Todo/JobSystem/JobSystem.hpp"

namespace Todo {
	JobSystem::JobSystem(const uint32_t threadCount)
	{
		m_Threads.reserve(threadCount);
		for (uint32_t i = 0; i < threadCount; ++i)
		{
			std::string name = "JobThread " + std::to_string(i);
			m_Threads.emplace_back(std::move(name), &JobSystem::Poll, this);
		}
		m_Running.store(true, std::memory_order_release);
	}

	JobSystem::~JobSystem()
	{
		m_Running.store(false, std::memory_order_release);
		m_Threads.clear();
	}

	void JobSystem::Poll()
	{
		//TODO: Poll jobs and execute them
	}
} // Todo