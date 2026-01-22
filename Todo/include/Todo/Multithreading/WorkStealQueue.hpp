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
#include "Todo/ThirdParty/wsq.hpp"

namespace Todo
{
    // This is a naive implementation of the Work Steal Queue.
    // Some better alternative exist, find & replace the core implementation with a better & more performant one.

    /// A queue meant to be used in parallel where a single queue can push and pop at the top of the queue and the
    /// rest of the threads can steal data at the bottom of it.
    /// @tparam T Type of the data in the queue.
    template <std::movable T>
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
        [[nodiscard]] bool empty() const;
        bool try_pop(T& data);
        bool try_steal(T& data);
        std::optional<T> pop();
        std::optional<T> steal();

    private:
        wsq::WorkStealingQueue<T> m_InternQueue;
    };

    template <std::movable T>
    void WorkStealQueue<T>::push(T data)
    {
        m_InternQueue.push(std::move(data));
    }

    template <std::movable T>
    bool WorkStealQueue<T>::empty() const
    {
        return m_InternQueue.empty();
    }

    template <std::movable T>
    bool WorkStealQueue<T>::try_pop(T& data)
    {
        if (auto result = m_InternQueue.pop())
        {
            data = std::move(result.value());
            return true;
        }
        return false;
    }

    template <std::movable T>
    bool WorkStealQueue<T>::try_steal(T& data)
    {
        if (auto result = m_InternQueue.steal())
        {
            data = std::move(result.value());
            return true;
        }
        return false;
    }

    template <std::movable T>
    std::optional<T> WorkStealQueue<T>::pop()
    {
        return m_InternQueue.pop();
    }

    template <std::movable T>
    std::optional<T> WorkStealQueue<T>::steal()
    {
        return m_InternQueue.steal();
    }
}
