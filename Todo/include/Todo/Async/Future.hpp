//
// Created by Sayama on 07/01/2026.
//

#pragma once
#include "SharedFuture.hpp"
#include "SharedResult.hpp"


namespace Todo {

	// The new future should be faster than the MSVC implementation and about fast as GCC/CLang implementation if atomic_16 is available.

	template <typename T>
	class Future {
	public:
		Future();
		~Future();
		Future(const Future &) = delete;
		Future &operator=(const Future &) = delete;
		Future(Future &&o) noexcept;
		Future &operator=(Future &&o) noexcept;
		void swap(Future &o) noexcept;

		Future(SharedResult<T> result);
	public:
		SharedFuture<T> share();

	public:
		[[nodiscard]] bool is_ready() const;
		[[nodiscard]] bool is_valid() const;
		void wait() const;
		[[nodiscard]] T get() const;

	private:
		SharedResult<T> m_Result;
	};

	template <typename T>
	Future<T>::Future() : m_Result(MakeSharedResult<T>()) {}

	template <typename T>
	Future<T>::~Future() = default;

	template <typename T>
	Future<T>::Future(Future&& o) noexcept : m_Result(MakeSharedResult<T>())
	{
		swap(o);
	}

	template <typename T>
	Future<T>& Future<T>::operator=(Future&& o) noexcept
	{
		swap(o);
		return *this;
	}

	template <typename T>
	void Future<T>::swap(Future& o) noexcept
	{
		std::swap(m_Result, o.m_Result);
	}

	template <typename T>
	Future<T>::Future(SharedResult<T> result)
	{
		m_Result = std::move(result);
	}

	template <typename T>
	SharedFuture<T> Future<T>::share()
	{
		return SharedFuture<T>(std::move(m_Result));
	}

	template <typename T>
	bool Future<T>::is_ready() const
	{
		return m_Result->is_ready();
	}

	template <typename T>
	bool Future<T>::is_valid() const
	{
		return m_Result->is_valid();
	}

	template <typename T>
	void Future<T>::wait() const
	{
		m_Result->wait_ready();
	}

	template <typename T>
	T Future<T>::get() const
	{
		std::optional<T> result = m_Result->wait_and_get();
		if (!result)
		{
			throw std::future_error(std::future_errc::future_already_retrieved);
		}
		return std::move(result.value());
	}

	// void implementation

	template <>
	class Future<void> {
	public:
		Future();
		~Future();
		Future(const Future &) = delete;
		Future &operator=(const Future &) = delete;
		Future(Future &&o) noexcept;
		Future &operator=(Future &&o) noexcept;
		void swap(Future &o) noexcept;

		Future(SharedResult<void> result);
	public:
		SharedFuture<void> share();

	public:
		[[nodiscard]] bool is_ready() const;
		[[nodiscard]] bool is_valid() const;
		void wait() const;
		void get() const;

	private:
		SharedResult<void> m_Result;
	};
} // namespace EID
