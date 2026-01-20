//
// Created by Sayama on 06/01/2026.
//

#pragma once

#include "SharedResult.hpp"

namespace Todo {

	template <typename T, typename TAlloc = TAllocator<T>>
	class SharedFuture {
	public:
		SharedFuture();
		~SharedFuture();
		SharedFuture(const SharedFuture &o);
		SharedFuture &operator=(const SharedFuture &o);
		SharedFuture(SharedFuture &&o) noexcept;
		SharedFuture &operator=(SharedFuture &&o) noexcept;
		void swap(SharedFuture &other) noexcept;

		explicit SharedFuture(SharedResult<T, TAlloc> result);

	public:
		[[nodiscard]] bool is_ready() const;
		[[nodiscard]] bool is_valid() const;
		void wait() const;
		[[nodiscard]] const T& get() const;

	private:
		SharedResult<T, TAlloc> m_Result;
	};

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::SharedFuture() : m_Result(MakeSharedResult<T, TAlloc>()) {}

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::~SharedFuture() = default;

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::SharedFuture(const SharedFuture& o) = default;

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>& SharedFuture<T, TAlloc>::operator=(const SharedFuture& o) = default;

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::SharedFuture(SharedFuture&& o) noexcept : m_Result(MakeSharedResult<T, TAlloc>())
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
		std::swap(m_Result, other.m_Result);
	}

	template <typename T, typename TAlloc>
	bool SharedFuture<T, TAlloc>::is_ready() const
	{
		return m_Result->is_ready();
	}

	template <typename T, typename TAlloc>
	bool SharedFuture<T, TAlloc>::is_valid() const
	{
		return m_Result->is_valid();
	}

	template <typename T, typename TAlloc>
	void SharedFuture<T, TAlloc>::wait() const
	{
		m_Result->wait_ready();
	}

	template <typename T, typename TAlloc>
	const T& SharedFuture<T, TAlloc>::get() const
	{
		if (!m_Result->is_valid())
		{
			throw std::future_error(std::future_errc::future_already_retrieved);
		}

		const T* ptr = m_Result->wait_and_get_ref();
		TODO_SASSERT(ptr != nullptr);
		return *ptr;
	}

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc>::SharedFuture(SharedResult<T, TAlloc> result) : m_Result(std::move(result))
	{
	}
} // namespace EID
