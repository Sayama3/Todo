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

#pragma once

#include "Todo/Core/Macros.hpp"
#include "Todo/Core/Concepts.hpp"
#include "Todo/Core/Logger.hpp"

#include "Todo/Async/Future.hpp"
#include "Todo/Async/SharedFuture.hpp"
#include "Todo/Async/Promise.hpp"
#include "Todo/Async/PackagedTask.hpp"

#include "Todo/Multithreading/Thread.hpp"
#include "Todo/Multithreading/Lockguard.hpp"
#include "Todo/Multithreading/SpinlockMutex.hpp"
#include "Todo/Multithreading/ThreadsafeStack.hpp"
#include "Todo/Multithreading/ThreadsafeQueue.hpp"

#include "Todo/JobSystem/JobSystem.hpp"

#include "Todo/JobSystem/ThreadPool.hpp"


namespace Todo {
	/**
	 * Get the number of thread available in the current CPU.
	 * @return the number of thread available in the current CPU.
	 */
	[[maybe_unused]] uint32_t GetAvailableThreadCount();
	[[maybe_unused]] uint32_t GetOptimalThreadCount();

	// Initialization
	void Initialize(std::unique_ptr<JobSystem> jobsystem);
	void Initialize(uint32_t threadCount);
	inline void Initialize() {Initialize(GetOptimalThreadCount());}
	void Destroy();

	// Sending Jobs


	// Utility Functions
	void WaitPhaseCompletion();
}