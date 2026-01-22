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

#include "SharedResult.hpp"

namespace Todo
{
    template <typename T>
    class SharedFuture
    {
    public:
        SharedFuture();
        ~SharedFuture();
        SharedFuture(const SharedFuture& o);
        SharedFuture& operator=(const SharedFuture& o);
        SharedFuture(SharedFuture&& o) noexcept;
        SharedFuture& operator=(SharedFuture&& o) noexcept;
        void swap(SharedFuture& other) noexcept;

        explicit SharedFuture(SharedResult<T> result);

    public:
        [[nodiscard]] bool is_ready() const;
        [[nodiscard]] bool is_valid() const;
        void wait() const;

        template <typename Rep, typename Period>
        std::future_status wait_for(const std::chrono::duration<Rep, Period>& duration) const;

        template <typename Clock, typename Duration>
        std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& time_point) const;

        [[nodiscard]] const T& get() const;

    private:
        SharedResult<T> m_Result;
    };

    template <typename T>
    SharedFuture<T>::SharedFuture() : m_Result(MakeSharedResult<T>())
    {
    }

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
    template <typename Rep, typename Period>
    std::future_status SharedFuture<T>::wait_for(const std::chrono::duration<Rep, Period>& duration) const
    {
        if (!m_Result) { throw std::future_error(std::future_errc::no_state); }
        return m_Result->wait_for(duration);
    }

    template <typename T>
    template <typename Clock, typename Duration>
    std::future_status SharedFuture<T>::wait_until(const std::chrono::time_point<Clock, Duration>& time_point) const
    {
        if (!m_Result) { throw std::future_error(std::future_errc::no_state); }
        return m_Result->wait_until(time_point);
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

    // Void Specialization


    template <>
    class SharedFuture<void>
    {
    public:
        SharedFuture();
        ~SharedFuture();
        SharedFuture(const SharedFuture& o);
        SharedFuture& operator=(const SharedFuture& o);
        SharedFuture(SharedFuture&& o) noexcept;
        SharedFuture& operator=(SharedFuture&& o) noexcept;
        void swap(SharedFuture& other) noexcept;

        explicit SharedFuture(SharedResult<void> result);

    public:
        [[nodiscard]] bool is_ready() const;
        [[nodiscard]] bool is_valid() const;
        void wait() const;

        template <typename Rep, typename Period>
        inline std::future_status wait_for(const std::chrono::duration<Rep, Period>& duration) const
        {
            if (!m_Result) { throw std::future_error(std::future_errc::no_state); }
            return m_Result->wait_for(duration);
        }

        template <typename Clock, typename Duration>
        inline std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& time_point) const
        {
            if (!m_Result) { throw std::future_error(std::future_errc::no_state); }
            return m_Result->wait_until(time_point);
        }

        void get() const;

    private:
        SharedResult<void> m_Result;
    };
} // namespace EID
