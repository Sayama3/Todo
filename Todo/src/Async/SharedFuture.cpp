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

#include "Todo/Async/SharedFuture.hpp"

namespace Todo
{
    SharedFuture<void>::SharedFuture() : m_Result(MakeSharedVoidResult())
    {
    }

    SharedFuture<void>::~SharedFuture() = default;

    SharedFuture<void>::SharedFuture(const SharedFuture& o) = default;

    SharedFuture<void>& SharedFuture<void>::operator=(const SharedFuture& o) = default;

    SharedFuture<void>::SharedFuture(SharedFuture&& o) noexcept : m_Result(MakeSharedVoidResult())
    {
        swap(o);
    }

    SharedFuture<void>& SharedFuture<void>::operator=(SharedFuture&& o) noexcept
    {
        swap(o);
        return *this;
    }

    void SharedFuture<void>::swap(SharedFuture& other) noexcept
    {
        std::swap(m_Result, other.m_Result);
    }

    bool SharedFuture<void>::is_ready() const
    {
        return m_Result->is_ready();
    }

    bool SharedFuture<void>::is_valid() const
    {
        return m_Result->is_valid();
    }

    void SharedFuture<void>::wait() const
    {
        m_Result->wait_ready();
    }

    void SharedFuture<void>::get() const
    {
        if (!m_Result->is_valid())
        {
            throw std::future_error(std::future_errc::future_already_retrieved);
        }

        m_Result->wait_and_get_ref();
    }

    SharedFuture<void>::SharedFuture(SharedResult<void> result) : m_Result(std::move(result))
    {
    }
}
