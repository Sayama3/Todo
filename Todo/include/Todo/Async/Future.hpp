//
// Created by Sayama on 07/01/2026.
//

#pragma once
#include "SharedFuture.hpp"
#include "SharedResult.hpp"


namespace Todo {

	// The new future should be faster than the MSVC implementation and about fast as GCC/CLang implementation if atomic_16 is available.

	template <typename T, typename TAlloc = TAllocator<T>>
	class Future {
	public:
		Future();
		~Future();
		Future(const Future &) = delete;
		Future &operator=(const Future &) = delete;
		Future(Future &&o) noexcept;
		Future &operator=(Future &&o) noexcept;
		void swap(Future &o) noexcept;

		Future(SharedResult<T, TAlloc> result);
	public:
		SharedFuture<T, TAlloc> share();

	public:
		[[nodiscard]] bool is_ready() const;
		[[nodiscard]] bool is_valid() const;
		void wait() const;
		[[nodiscard]] T get() const;

	private:
		SharedResult<T, TAlloc> m_Result;
	};

	template <typename T, typename TAlloc>
	Future<T, TAlloc>::Future() : m_Result(MakeSharedResult<T,TAlloc>()) {}

	template <typename T, typename TAlloc>
	Future<T, TAlloc>::~Future() = default;

	template <typename T, typename TAlloc>
	Future<T, TAlloc>::Future(Future&& o) noexcept : m_Result(MakeSharedResult<T, TAlloc>())
	{
		swap(o);
	}

	template <typename T, typename TAlloc>
	Future<T, TAlloc>& Future<T, TAlloc>::operator=(Future&& o) noexcept
	{
		swap(o);
		return *this;
	}

	template <typename T, typename TAlloc>
	void Future<T, TAlloc>::swap(Future& o) noexcept
	{
		std::swap(m_Result, o.m_Result);
	}

	template <typename T, typename TAlloc>
	Future<T, TAlloc>::Future(SharedResult<T, TAlloc> result)
	{
		m_Result = std::move(result);
	}

	template <typename T, typename TAlloc>
	SharedFuture<T, TAlloc> Future<T, TAlloc>::share()
	{
		return SharedFuture<T, TAlloc>(std::move(m_Result));
	}

	template <typename T, typename TAlloc>
	bool Future<T, TAlloc>::is_ready() const
	{
		return m_Result->is_ready();
	}

	template <typename T, typename TAlloc>
	bool Future<T, TAlloc>::is_valid() const
	{
		return m_Result->is_valid();
	}

	template <typename T, typename TAlloc>
	void Future<T, TAlloc>::wait() const
	{
		m_Result->wait();
	}

	template <typename T, typename TAlloc>
	T Future<T, TAlloc>::get() const
	{
		std::optional<T> result = m_Result->wait_and_get();
		if (!result)
		{
			throw std::future_error(std::future_errc::future_already_retrieved);
		}
		return std::move(result.value());
	}
} // namespace EID
