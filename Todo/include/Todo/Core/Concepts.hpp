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

namespace Todo {
	template<typename Mut>
	concept CMutex = requires(Mut mut)
	{
		{ mut.lock() };
		{ mut.unlock() };
		{ mut.try_lock() } -> std::convertible_to<bool>;
	};

	template<typename Mut>
	concept CSharedMutex = CMutex<Mut> && requires(Mut mut)
	{
		{ mut.lock_shared() };
		{ mut.unlock_shared() };
		{ mut.try_lock_shared() } -> std::convertible_to<bool>;
	};

	template<typename T>
	concept Trivial =
			std::is_trivially_copyable_v<T> == true &&
			std::is_trivially_default_constructible_v<T> == true &&
			std::is_trivially_copy_constructible_v<T> == true &&
			std::is_trivially_move_constructible_v<T> == true;

	template<typename T>
	concept StdHashable = requires(T a) {
		{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
	};

	template <typename HashFunc, typename T>
	concept HashFor = std::regular_invocable<HashFunc, T> && std::convertible_to<std::invoke_result_t<HashFunc, T>, size_t>;

	template<typename C>
	concept StdClock = std::chrono::is_clock_v<C>;

	template<typename F, typename ... Args>
	concept MovableInvocable = std::move_constructible<F> && std::invocable<F, Args...>;

	template<typename F, typename ... Args>
	concept MovableVoidInvocable = MovableInvocable<F, void>;

	template<typename T>
	concept LessThanComparable = requires(T a, T b) {
		{a < b} -> std::convertible_to<bool>;
	};

	template<typename F>
	concept VoidInvocable = std::invocable<F,void>;
}
