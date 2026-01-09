//
// Created by ianpo on 01/09/2024.
//

#pragma once

#include "Concepts.hpp"
#include "Lockguard.hpp"

namespace Todo
{

	/**
	 * Overload of any mutex adding among other the possibility to fetch
	 * a Todo::Lockguard or a std::unique_lock from the member functions
	 * @tparam Mut The mutex type we want to overload.
	 */
	template<CMutex Mut>
	class TMutex
	{
	public:
		TMutex() = default;
		~TMutex() = default;
	public:
		TMutex(const TMutex&) = delete;
		TMutex& operator=(const TMutex&) = delete;
		TMutex(TMutex&&) = delete;
		TMutex& operator=(TMutex&&) = delete;
	public:
		void lock() { m_Mutex.lock(); }
		void unlock() { m_Mutex.unlock(); }
		bool try_lock() noexcept {return m_Mutex.try_lock();}

		Mut& get() {return m_Mutex;}
		const Mut& get() const {return m_Mutex;}
	public:
		[[maybe_unused]] [[nodiscard]] Lockguard<TMutex<Mut>> Guard() { return {this}; }
		[[maybe_unused]] [[nodiscard]] UniqueLockguard<TMutex<Mut>> UniqueGuard() { return {*this }; }
	protected:
		Mut m_Mutex;
	};

	template<CSharedMutex Mut>
	class TSharedMutex : public TMutex<Mut>
	{
	public:
		TSharedMutex() = default;
		~TSharedMutex() = default;
	public:
		void lock_shared() { TMutex<Mut>::m_Mutex.lock_shared(); }
		void unlock_shared() { TMutex<Mut>::m_Mutex.unlock_shared(); }
		bool try_lock_shared() noexcept {return TMutex<Mut>::m_Mutex.try_lock_shared();}
	public:
		[[maybe_unused]] [[nodiscard]] SharedLockguard<TSharedMutex<Mut>> SharedGuard() { return {*this }; }
	};
}