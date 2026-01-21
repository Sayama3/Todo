//
// Created by ianpo on 12/01/2026.
//

#pragma once
#include "Todo/Async/Future.hpp"
#include "Todo/Async/PackagedTask.hpp"
#include "Todo/Multithreading/Thread.hpp"
#include "Todo/Multithreading/ThreadsafeQueue.hpp"
#include "Todo/Core/FunctionWrapper.hpp"

namespace Todo
{
    class ThreadPool
    {
    private:
        using Task = FunctionWrapper;
    public:
        ThreadPool();
        ThreadPool(uint32_t thread_count);
        ~ThreadPool();
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
    public:

    private:
        void worker_thread();

    public:
        template <std::invocable Func>
        Future<std::invoke_result_t<Func>> Submit(Func f)
        {
            typedef typename std::invoke_result_t<Func> result_type;
            PackagedTask<result_type()> task(std::move(f));
            Future<result_type> future = task.get_future();
            m_WorkQueue.push(std::move(task));
            return future;
        }
    private:
        std::atomic_bool m_Done{false};
        ThreadsafeQueue<Task> m_WorkQueue;
        std::vector<Thread> m_Threads;
    };

}
