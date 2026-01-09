//
// Created by Sayama on 31/08/2024.
//

#include "Todo/Multithreading/SpinlockMutex.hpp"

namespace Todo {
	SpinlockMutex::SpinlockMutex() = default;

	SpinlockMutex::~SpinlockMutex() = default;

	void SpinlockMutex::lock() {
		while (m_Flag.test_and_set(std::memory_order_acquire));
	}

	void SpinlockMutex::unlock() {
		m_Flag.clear(std::memory_order_release);
	}

	bool SpinlockMutex::try_lock() noexcept {
		return !m_Flag.test_and_set(std::memory_order_acquire);
	}

	Lockguard<SpinlockMutex> SpinlockMutex::Guard() {
		return Lockguard<SpinlockMutex>{this};
	}

	UniqueLockguard<SpinlockMutex> SpinlockMutex::UniqueGuard() {
		return UniqueLockguard<SpinlockMutex>{*this};
	}
}
