//
// Created by ianpo on 31/08/2024.
//

#pragma once

#include "Lockguard.hpp"

namespace Todo {
	class SpinlockMutex;
	using SpinLockGuard = Lockguard<SpinlockMutex>;
	using UniqueSpinLockGuard = UniqueLockguard<SpinlockMutex>;

	class SpinlockMutex {
	public:
		SpinlockMutex();
		SpinlockMutex(const SpinlockMutex&) = delete;
		SpinlockMutex& operator=(const SpinlockMutex&) = delete;
	public:
		void lock();
		void unlock();
		bool try_lock() noexcept;
	public:
		[[maybe_unused]] [[nodiscard]] SpinLockGuard Guard() { return SpinLockGuard{this}; }
		[[maybe_unused]] [[nodiscard]] UniqueSpinLockGuard UniqueGuard() { return UniqueSpinLockGuard{*this}; }
	private:
		std::atomic_flag m_Flag;
	};

} // Todo
