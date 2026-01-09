//
// Created by Sayama on 07/08/2025.
//

#pragma once

namespace Todo {
	template<typename Mut>
	concept CMutex = requires(Mut mut)
	{
		{ mut.lock() };
		{ mut.unlock() };
		{ mut.try_lock() } noexcept -> std::convertible_to<bool>;
	};

	template<typename Mut>
	concept CSharedMutex = CMutex<Mut> && requires(Mut mut)
	{
		{ mut.lock_shared() };
		{ mut.unlock_shared() };
		{ mut.try_lock_shared() } noexcept -> std::convertible_to<bool>;
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
}
