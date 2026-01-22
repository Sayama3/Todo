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

#include "Todo/Todo.hpp"
#include "Todo/Core/Macros.hpp"
#include "Todo/Core/Helper.hpp"
#include "Todo/Core/Logger.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

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
		return (threadCount - TODO_REMAINING_THREAD) > TODO_MIN_THREAD_COUNT ? (threadCount - TODO_REMAINING_THREAD) : TODO_MIN_THREAD_COUNT;
#else
		return threadCount;
#endif
	}

	void Initialize(uint32_t threadCount)
	{
		if(s_JobSystem) {
			TODO_ERROR("The Job System is already initialized. Replacing the old one with the new one.");
		}
		s_JobSystem = std::make_unique<JobSystem>(threadCount);

		TODO_INFO("Todo Initialized with "s + std::to_string(threadCount) + " threads."s);
	}

	void Initialize(std::unique_ptr<JobSystem> jobsystem) {
		if(s_JobSystem) {
			TODO_ERROR("The Job System is already initialized. Replacing the old one with the new one.");
		}
		s_JobSystem = std::move(jobsystem);
	}

	void Destroy()
	{
		s_JobSystem.reset(nullptr);
		TODO_INFO("Todo Destroy.");
	}
}