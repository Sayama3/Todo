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

#include "Todo/Core/Concepts.hpp"

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
