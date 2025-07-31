//
// Created by ianpo on 31/08/2024.
//

#pragma once

#include "Lockguard.hpp"

namespace Todo {

	class SpinlockMutex {
	public:
		SpinlockMutex();
		~SpinlockMutex();
		SpinlockMutex(const SpinlockMutex&) = delete;
		SpinlockMutex& operator=(const SpinlockMutex&) = delete;
	public:
		void lock();
		void unlock();
		bool try_lock() noexcept;
	public:
		[[maybe_unused]] [[nodiscard]] Lockguard<SpinlockMutex> Guard();
		[[maybe_unused]] [[nodiscard]] UniqueLockguard<SpinlockMutex> UniqueGuard();

	private:
		std::atomic_flag m_Flag=ATOMIC_FLAG_INIT;
	};

	using SpinLockGuard = Lockguard<SpinlockMutex>;
	using UniqueSpinLockGuard = UniqueLockguard<SpinlockMutex>;

}
