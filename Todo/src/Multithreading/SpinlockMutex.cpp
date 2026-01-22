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

#include "Todo/Multithreading/SpinlockMutex.hpp"

namespace Todo {
	SpinlockMutex::SpinlockMutex() = default;

	SpinlockMutex::~SpinlockMutex() = default;

	void SpinlockMutex::lock() noexcept {
		while (m_Flag.test_and_set(std::memory_order_acquire)) {}
	}

	void SpinlockMutex::unlock() noexcept {
		m_Flag.clear(std::memory_order_release);
	}

	bool SpinlockMutex::try_lock() noexcept {
		return !m_Flag.test_and_set(std::memory_order_acquire);
	}

	Lockguard<SpinlockMutex> SpinlockMutex::Guard() {
		return Lockguard<SpinlockMutex>{this};
	}

	UniqueLockguard<SpinlockMutex> SpinlockMutex::UniqueGuard() {
		return UniqueLockguard<SpinlockMutex>{*this};
	}
}
