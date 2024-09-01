//
// Created by ianpo on 31/08/2024.
//

#pragma once

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

} // Todo
