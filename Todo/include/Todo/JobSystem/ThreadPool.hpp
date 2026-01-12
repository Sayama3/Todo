//
// Created by ianpo on 12/01/2026.
//

#pragma once
#include "Todo/Multithreading/Thread.hpp"
#include "Todo/Multithreading/ThreadsafeQueue.hpp"

namespace Todo
{
    class ThreadPool
    {
    private:
        using Task = std::function<void()>;
    public:
        ThreadPool();
        ThreadPool(uint32_t thread_count);
        ~ThreadPool();
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
    public:

    private:
        void worker_thread();

        template<typename Func>
        void Submit(Func f);
    private:
        std::atomic_bool m_Done{false};
        ThreadsafeQueue<Task> m_WorkQueue;
        std::vector<Thread> m_Threads;
    };

    template <typename Func>
    inline void ThreadPool::Submit(Func f)
    {
        m_WorkQueue.push(std::function<void()>(f));
    }
}
