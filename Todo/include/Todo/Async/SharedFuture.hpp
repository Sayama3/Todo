//
// Created by Sayama on 06/01/2026.
//

#pragma once

#include "SharedResult.hpp"

namespace Todo {

	template<typename T, typename TAlloc>
	class SharedFuture {
	public:
		SharedFuture();
		~SharedFuture();
		SharedFuture(const SharedFuture &o);
		SharedFuture &operator=(const SharedFuture &o);
		SharedFuture(SharedFuture &&o) noexcept;
		SharedFuture &operator=(SharedFuture &&o) noexcept;
		void swap(SharedFuture &other) noexcept;

		SharedFuture(SharedResult<T, TAlloc> result);

	public:
		[[nodiscard]] bool is_ready() const;
		void wait() const;
		[[nodiscard]] const T& get() const;

	private:
		SharedResult<T, TAlloc> result;
	};

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::SharedFuture() = default;

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::~SharedFuture() = default;

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::SharedFuture(const SharedFuture& o) = default;

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>& SharedFuture<T, TAlloc>::operator=(const SharedFuture& o) = default;

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::SharedFuture(SharedFuture&& o) noexcept
	{
		swap(o);
	}

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>& SharedFuture<T, TAlloc>::operator=(SharedFuture&& o) noexcept
	{
		swap(o);
		return *this;
	}

	template <typename T, typename TAlloc>
	void SharedFuture<T, TAlloc>::swap(SharedFuture& other) noexcept
	{
		std::swap(result, other.result);
	}

	template <typename T, typename TAlloc>
	bool SharedFuture<T, TAlloc>::is_ready() const
	{
		return result.is_ready();
	}

	template <typename T, typename TAlloc>
	void SharedFuture<T, TAlloc>::wait() const
	{
		result.wait_ready();
	}

	template <typename T, typename TAlloc>
	const T& SharedFuture<T, TAlloc>::get() const
	{
		return result.wait_and_get();
	}

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::SharedFuture(SharedResult<T, TAlloc> result) : result(std::move(result))
	{
	}
} // namespace EID
