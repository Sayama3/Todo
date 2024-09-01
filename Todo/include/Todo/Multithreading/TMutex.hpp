//
// Created by ianpo on 01/09/2024.
//

#pragma once

#include "Lockguard.hpp"

namespace Todo
{


	/**
	 * Overload of any mutex adding among other the possibility to fetch
	 * a Todo::Lockguard or a std::unique_lock from the member functions
	 * @tparam Mut The mutex type we want to overload.
	 */
	template<typename Mut>
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
	public:
		[[maybe_unused]] [[nodiscard]] Lockguard<TMutex<Mut>> Guard() { return {this}; }
		[[maybe_unused]] [[nodiscard]] UniqueLockguard<TMutex<Mut>> UniqueGuard() { return {*this }; }
	private:
		Mut m_Mutex;
	};
}