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

#include "Todo/Core/Concepts.hpp"
#include "Todo/Multithreading/Mutex.hpp"
#include "Todo/Multithreading/TMutex.hpp"

namespace Todo
{

    // This is a naive implementation of the Work Steal Queue.
    // Some better alternative exist, find & replace the core implementation with a better & more performant one.

    /// A queue meant to be used in parallel where a single queue can push and pop at the top of the queue and the
    /// rest of the thread can steal data at the bottom of it.
    /// @tparam T Type of the data in the queue.
    template<std::copyable T, CMutex Mut = std::mutex>
    class WorkStealQueue
    {
    public:
        WorkStealQueue() = default;
        ~WorkStealQueue() = default;
        WorkStealQueue(WorkStealQueue&) = delete;
        WorkStealQueue(const WorkStealQueue&) = delete;
        WorkStealQueue& operator=(const WorkStealQueue&) = delete;
    public:
        void push(T data);
        bool empty() const;
        bool try_pop(T& data);
        bool try_steal(T& data);
    private:
        std::deque<T> m_Queue;
        mutable TMutex<Mut> m_Mutex;
    };

    template <std::copyable T, CMutex Mut>
    void WorkStealQueue<T, Mut>::push(T data)
    {
        auto lock = m_Mutex.UniqueGuard();
        m_Queue.push_front(std::move(data));
    }

    template <std::copyable T, CMutex Mut>
    bool WorkStealQueue<T, Mut>::empty() const
    {
        auto lock = m_Mutex.UniqueGuard();
        return m_Queue.empty();
    }

    template <std::copyable T, CMutex Mut>
    bool WorkStealQueue<T, Mut>::try_pop(T& data)
    {
        auto lock = m_Mutex.UniqueGuard();
        if (m_Queue.empty())
        {
            return false;
        }
        data = std::move(m_Queue.front());
        m_Queue.pop_front();
        return true;
    }

    template <std::copyable T, CMutex Mut>
    bool WorkStealQueue<T, Mut>::try_steal(T& data)
    {
        auto lock = m_Mutex.UniqueGuard();
        if (m_Queue.empty())
        {
            return false;
        }
        data = std::move(m_Queue.back());
        m_Queue.pop_back();
        return true;
    }
}
