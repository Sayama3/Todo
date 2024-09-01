//
// Created by ianpo on 30/08/2024.
//

#include "Todo/JobSystem/JobSystem.hpp"

namespace Todo {
	JobSystem::JobSystem(const uint32_t threadCount)
	{
		m_Threads.reserve(threadCount);
		for (int i = 0; i < threadCount; ++i)
		{
			m_Threads.emplace_back(&JobSystem::Poll, this);
		}
	}

	JobSystem::~JobSystem()
	{
		m_Running.store(false, std::memory_order_acq_rel);
		m_Threads.clear();
	}

	void JobSystem::Poll()
	{
	}
} // Todo