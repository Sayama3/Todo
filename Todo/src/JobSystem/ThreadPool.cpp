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

#include <memory>

#include "Todo/JobSystem/ThreadPool.hpp"

namespace Todo
{
    ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency())
    {
    }

    ThreadPool::ThreadPool(const uint32_t thread_count) : m_Done(false)
    {
        try
        {
            m_Queues.reserve(thread_count);
            m_Threads.reserve(thread_count);

            for (uint32_t i = 0; i < thread_count; ++i)
            {
                m_Queues.push_back(std::make_unique<work_steal_queue>());
            }

            for (uint32_t i = 0; i < thread_count; ++i)
            {
                m_Threads.emplace_back(&ThreadPool::worker_thread, this, i);
            }
        }
        catch (...)
        {
            m_Done.store(true, std::memory_order_seq_cst);
            throw;
        }
    }

    ThreadPool::~ThreadPool()
    {
        m_Done.store(true);
    }

    void ThreadPool::worker_thread(const uint32_t id)
    {
        l_Index = id;
        l_LocalWorkQueue = m_Queues[l_Index].get();

        while (!m_Done.load(std::memory_order_relaxed))
        {
            run_pending_task();
        }
    }

    void ThreadPool::run_pending_task()
    {
        Task task;
        if (pop_task_from_local_queue(task))
        {
            task();
        }
        else if (pop_task_from_pool_queue(task))
        {
            task();
        }
        else if (pop_task_from_other_thread_queue(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }

    bool ThreadPool::pop_task_from_local_queue(Task& task)
    {
        if (l_LocalWorkQueue)
        {
            return l_LocalWorkQueue->try_pop(task);
        }
        return false;
    }

    bool ThreadPool::pop_task_from_pool_queue(Task& task)
    {
        return m_WorkQueue.try_pop(task);
    }

    bool ThreadPool::pop_task_from_other_thread_queue(Task& task)
    {
        for (uint32_t i = 0; i < m_Queues.size(); ++i)
        {
            const uint32_t index = (l_Index + i) % m_Queues.size();
            if (m_Queues[index] && m_Queues[index]->try_steal(task))
            {
                return true;
            }
        }
        return false;
    }
}
