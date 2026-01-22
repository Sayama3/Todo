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
        using global_queue_type = ThreadsafeQueue<Task>;
        using local_queue_type = std::queue<Task>;

    public:
        ThreadPool();
        ThreadPool(uint32_t thread_count);
        ~ThreadPool();
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;
    public:

    private:
        void worker_thread();
        void run_pending_task();
    public:
        template <std::invocable Func>
        Future<std::invoke_result_t<Func>> Submit(Func f)
        {
            using result_type = typename std::invoke_result_t<Func>;
            PackagedTask<result_type()> task(std::move(f));
            Future<result_type> future = task.get_future();

            if (local_work_queue)
            {
                local_work_queue->push(std::move(task));
            }
            else
            {
                m_WorkQueue.push(std::move(task));
            }
            return future;
        }
    private:
        std::atomic_bool m_Done{false};
        global_queue_type m_WorkQueue;
        inline static thread_local std::unique_ptr<local_queue_type> local_work_queue{};
        std::vector<Thread> m_Threads;
    };

}
