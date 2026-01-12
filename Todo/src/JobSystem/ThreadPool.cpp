//
// Created by ianpo on 12/01/2026.
//

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
        while (!m_Done.load(std::memory_order_seq_cst))
        {
            Task task;
            if (m_WorkQueue.try_pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }
}