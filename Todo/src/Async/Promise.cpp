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

#include "Todo/Async/Promise.hpp"

namespace Todo
{
    Promise<void>::Promise() : m_Result(MakeSharedVoidResult())
    {
    }

    Promise<void>::~Promise() = default;

    Promise<void>::Promise(Promise&& o) noexcept : m_Result(MakeSharedVoidResult())
    {
        swap(o);
    }

    Promise<void>& Promise<void>::operator=(Promise&& o) noexcept
    {
        swap(o);
        return *this;
    }

    void Promise<void>::swap(Promise& o) noexcept
    {
        std::swap(m_Result, o.m_Result);
    }

    Future<void> Promise<void>::get_future()
    {
        return Future<void>{m_Result};
    }

    void Promise<void>::set_value()
    {
        m_Result->set_value();
    }

    void Promise<void>::set_exception(std::exception_ptr p)
    {
        m_Result->set_exception(p);
    }

    bool Promise<void>::is_valid() const
    {
        return m_Result->is_valid();
    }

    bool Promise<void>::is_ready() const
    {
        return m_Result->is_ready();
    }
}
