//
// Created by ianpo on 31/08/2024.
//

#pragma once

#include "Concepts.hpp"

namespace Todo {

	template<typename Mutex>
	class Lockguard
	{
	public:
		Lockguard(Mutex& mutex) : m_Mutex(&mutex) {m_Mutex->lock();}
		Lockguard(Mutex* mutex) : m_Mutex(mutex) {m_Mutex->lock();}
		~Lockguard() {m_Mutex->unlock();}

		Lockguard& operator=(Lockguard&& lft) noexcept { swap(lft); return *this;}
		Lockguard(Lockguard&& lft) noexcept { swap(lft); }

		Lockguard& operator=(const Lockguard&) = delete;
		Lockguard(const Lockguard&) = delete;
	public:
		void swap(Lockguard& other) {std::swap(m_Mutex, other.m_Mutex);}
	private:
		Mutex* m_Mutex {nullptr};
	};

	template<typename Mut>
	using UniqueLockguard = std::unique_lock<Mut>;


	template<typename Mut>
	using SharedLockguard = std::shared_lock<Mut>;

	///
	/// Lockguard validating at compile time if the type provided is valid.
	/// Some time, it might still be needed to use the original "Todo::Lockguard".
	///
	template<CMutex Mutex>
	using CstLockguard = Lockguard<Mutex>;

	///
	/// UniqueLockguard validating at compile time if the type provided is valid.
	/// Some time, it might still be needed to use the original "Todo::UniqueLockguard".
	///
	template<CMutex Mutex>
	using CstUniqueLockguard = UniqueLockguard<Mutex>;

	///
	/// SharedLockguard validating at compile time if the type provided is valid.
	/// Some time, it might still be needed to use the original "Todo::SharedLockguard".
	///
	template<CSharedMutex Mutex>
	using CstSharedLockguard = SharedLockguard<Mutex>;

} // Todo
