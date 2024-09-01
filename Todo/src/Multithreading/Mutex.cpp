//
// Created by ianpo on 01/09/2024.
//

#include "Todo/Multithreading/Mutex.hpp"

namespace Todo {
	void Mutex::lock()
	{
		std::mutex other;
		m_Mutex.lock();
	}

	bool Mutex::try_lock() noexcept {
		return m_Mutex.try_lock();
	}

	void Mutex::unlock()
	{
		m_Mutex.unlock();
	}

} // Todo