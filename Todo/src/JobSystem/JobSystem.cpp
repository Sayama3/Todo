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