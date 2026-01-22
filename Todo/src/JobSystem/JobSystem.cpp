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
	JobSystem::JobSystem() : JobSystem(std::thread::hardware_concurrency())
	{
	}

	JobSystem::JobSystem(const uint32_t threadCount) : thread_count(threadCount)
	{
	}

	JobSystem::~JobSystem()
	{
		if (m_ThreadPool)
		{
			m_ThreadPool->Stop();
		}

		for (auto& dedicated : m_DedicatedThreads)
		{
			dedicated.second.thread->request_stop();
			dedicated.second.thread->try_join();
		}
	}

	JobSystem& JobSystem::AddDedicatedThread(std::function<void()> func, uint32_t& id)
	{
		id = ++g_ID;
		m_DedicatedThreads.insert({id, DedicatedThread{func, nullptr}});

		return *this;
	}

	void JobSystem::Run()
	{
		const int64_t remainingThread = static_cast<int64_t>(thread_count) - static_cast<int64_t>(m_DedicatedThreads.size());
		if (remainingThread <= 0)
		{
			const std::string errorMessage = "We are missing at least " + std::to_string((-remainingThread) + 1) + " threads.";
			throw std::runtime_error(errorMessage);
		}
		m_ThreadPool = std::make_unique<ThreadPool>(static_cast<uint32_t>(remainingThread));
	}
} // Todo