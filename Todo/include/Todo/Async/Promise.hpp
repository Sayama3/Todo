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

#include "Future.hpp"
#include "SharedResult.hpp"

namespace Todo
{
    template <class R>
    class Promise
    {
    public:
        Promise();
        ~Promise();
        Promise(const Promise&) = delete;
        Promise& operator=(const Promise&) = delete;
        Promise(Promise&& o) noexcept;
        Promise& operator=(Promise&& o) noexcept;
        void swap(Promise& o) noexcept;

    public:
        Todo::Future<R> get_future();

        void set_value(const R& value);
        void set_value(R&& value);
        void set_exception(std::exception_ptr p);

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] bool is_ready() const;

    private:
        SharedResult<R> m_Result;
    };

    template <class R>
    Promise<R>::Promise() : m_Result(MakeSharedResult<R>())
    {
    }

    template <class R>
    Promise<R>::~Promise() = default;

    template <class R>
    Promise<R>::Promise(Promise&& o) noexcept : m_Result(MakeSharedResult<R>())
    {
        swap(o);
    }

    template <class R>
    Promise<R>& Promise<R>::operator=(Promise&& o) noexcept
    {
        swap(o);
        return *this;
    }

    template <class R>
    void Promise<R>::swap(Promise& o) noexcept
    {
        std::swap(m_Result, o.m_Result);
    }

    template <class R>
    Future<R> Promise<R>::get_future()
    {
        return Future<R>{m_Result};
    }

    template <class R>
    void Promise<R>::set_value(const R& value)
    {
        m_Result->set_value(value);
    }

    template <class R>
    void Promise<R>::set_value(R&& value)
    {
        m_Result->set_value(std::move(value));
    }

    template <class R>
    void Promise<R>::set_exception(std::exception_ptr p)
    {
        m_Result->set_exception(p);
    }

    template <class R>
    bool Promise<R>::is_valid() const
    {
        return m_Result->is_valid();
    }

    template <class R>
    bool Promise<R>::is_ready() const
    {
        return m_Result->is_ready();
    }

    // Void Promise


    template <>
    class Promise<void>
    {
    public:
        Promise();
        ~Promise();
        Promise(const Promise&) = delete;
        Promise& operator=(const Promise&) = delete;
        Promise(Promise&& o) noexcept;
        Promise& operator=(Promise&& o) noexcept;
        void swap(Promise& o) noexcept;

    public:
        Todo::Future<void> get_future();

        void set_value();
        void set_exception(std::exception_ptr p);

        template <typename... Args>
        void set_value(Args&&... args)
        {
            m_Result->set_value();
        }

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] bool is_ready() const;

    private:
        SharedResult<void> m_Result;
    };
}
