//
// Created by ianpo on 12/01/2026.
//

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
