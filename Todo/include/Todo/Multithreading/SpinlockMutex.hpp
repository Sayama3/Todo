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

#include "Lockguard.hpp"

namespace Todo {

	class SpinlockMutex {
	public:
		SpinlockMutex();
		~SpinlockMutex();
		SpinlockMutex(const SpinlockMutex&) = delete;
		SpinlockMutex& operator=(const SpinlockMutex&) = delete;
	public:
		void lock() noexcept;
		void unlock() noexcept;
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
