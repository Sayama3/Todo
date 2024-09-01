//
// Created by ianpo on 31/08/2024.
//

#include "Todo/Multithreading/SpinlockMutex.hpp"

namespace Todo
{

	SpinlockMutex::SpinlockMutex() : m_Flag() {}

	void SpinlockMutex::lock()
	{
		while (m_Flag.test_and_set(std::memory_order_release));
	}

	void SpinlockMutex::unlock()
	{
		m_Flag.clear(std::memory_order_release);
	}

	bool SpinlockMutex::try_lock() noexcept {
		return !m_Flag.test_and_set(std::memory_order_release);
	}
}