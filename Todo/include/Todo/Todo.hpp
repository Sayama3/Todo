//
// Created by ianpo on 30/08/2024.
//

#pragma once

#include "Todo/Core/Macros.hpp"
#include "Todo/Core/Logger.hpp"
#include "Todo/Multithreading/Thread.hpp"
#include "Todo/Multithreading/Lockguard.hpp"
#include "Todo/Multithreading/SpinlockMutex.hpp"
#include "Todo/Multithreading/ThreadsafeStack.hpp"
#include "Todo/Multithreading/SafeQueue.hpp"
#include "Todo/JobSystem/JobSystem.hpp"


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