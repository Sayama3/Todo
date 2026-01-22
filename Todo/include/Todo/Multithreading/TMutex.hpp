//    Copyright 2026 https://github.com/Sayama3
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "../Core/Concepts.hpp"
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
		[[maybe_unused]] [[nodiscard]] Lockguard<TMutex<Mut>> Guard() { return Lockguard<TMutex<Mut>>(*this); }
		[[maybe_unused]] [[nodiscard]] UniqueLockguard<TMutex<Mut>> UniqueGuard() { return UniqueLockguard<TMutex<Mut>>(*this ); }
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