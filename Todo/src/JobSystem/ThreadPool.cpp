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
    ThreadPool::ThreadPool() : m_Done(false)
    {
        uint32_t const thread_count = std::thread::hardware_concurrency();
        try
        {
            for (uint32_t i = 0; i < thread_count; ++i)
            {
                m_Threads.emplace_back(&ThreadPool::worker_thread, this);
            }
        }
        catch (...)
        {
            m_Done.store(true, std::memory_order_seq_cst);
            throw;
        }
    }

    ThreadPool::ThreadPool(const uint32_t thread_count) : m_Done(false)
    {
        try
        {
            for (uint32_t i = 0; i < thread_count; ++i)
            {
                m_Threads.emplace_back(&ThreadPool::worker_thread, this);
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

    void ThreadPool::worker_thread()
    {
        local_work_queue = std::make_unique<local_queue_type>();

        while (!m_Done.load(std::memory_order_relaxed))
        {
            run_pending_task();
        }
    }

    void ThreadPool::run_pending_task()
    {
        Task task;
        if (local_work_queue && !local_work_queue->empty())
        {
            task = std::move(local_work_queue->front());
            local_work_queue->pop();
            task();
        }
        else if (m_WorkQueue.try_pop(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}
