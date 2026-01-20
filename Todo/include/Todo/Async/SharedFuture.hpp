//
// Created by Sayama on 06/01/2026.
//

#pragma once

#include "SharedResult.hpp"

namespace Todo {

	template <typename T>
	class SharedFuture {
	public:
		SharedFuture();
		~SharedFuture();
		SharedFuture(const SharedFuture &o);
		SharedFuture &operator=(const SharedFuture &o);
		SharedFuture(SharedFuture &&o) noexcept;
		SharedFuture &operator=(SharedFuture &&o) noexcept;
		void swap(SharedFuture &other) noexcept;

		explicit SharedFuture(SharedResult<T> result);

	public:
		[[nodiscard]] bool is_ready() const;
		[[nodiscard]] bool is_valid() const;
		void wait() const;
		[[nodiscard]] const T& get() const;

	private:
		SharedResult<T> m_Result;
	};

	template <typename T>
	SharedFuture<T>::SharedFuture() : m_Result(MakeSharedResult<T>()) {}

	template <typename T>
	SharedFuture<T>::~SharedFuture() = default;

	template <typename T>
	SharedFuture<T>::SharedFuture(const SharedFuture& o) = default;

	template <typename T>
	SharedFuture<T>& SharedFuture<T>::operator=(const SharedFuture& o) = default;

	template <typename T>
	SharedFuture<T>::SharedFuture(SharedFuture&& o) noexcept : m_Result(MakeSharedResult<T>())
	{
		swap(o);
	}

	template <typename T>
	SharedFuture<T>& SharedFuture<T>::operator=(SharedFuture&& o) noexcept
	{
		swap(o);
		return *this;
	}

	template <typename T>
	void SharedFuture<T>::swap(SharedFuture& other) noexcept
	{
		std::swap(m_Result, other.m_Result);
	}

	template <typename T>
	bool SharedFuture<T>::is_ready() const
	{
		return m_Result->is_ready();
	}

	template <typename T>
	bool SharedFuture<T>::is_valid() const
	{
		return m_Result->is_valid();
	}

	template <typename T>
	void SharedFuture<T>::wait() const
	{
		m_Result->wait_ready();
	}

	template <typename T>
	const T& SharedFuture<T>::get() const
	{
		if (!m_Result->is_valid())
		{
			throw std::future_error(std::future_errc::future_already_retrieved);
		}

		const T* ptr = m_Result->wait_and_get_ref();
		TODO_SASSERT(ptr != nullptr);
		return *ptr;
	}

	template <typename T>
	SharedFuture<T>::SharedFuture(SharedResult<T> result) : m_Result(std::move(result))
	{
	}
} // namespace EID
