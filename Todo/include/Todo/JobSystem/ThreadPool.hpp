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
#include "Todo/Multithreading/WorkStealQueue.hpp"

namespace Todo
{
    class ThreadPool
    {
    private:
        using Task = FunctionWrapper;
        using work_steal_queue = WorkStealQueue<Task>;
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
        void worker_thread(uint32_t index);
        void run_pending_task();

    private:
        bool pop_task_from_local_queue(Task& task);
        bool pop_task_from_pool_queue(Task& task);
        bool pop_task_from_other_thread_queue(Task& task);

    public:
        template <std::invocable Func>
        Future<std::invoke_result_t<Func>> Submit(Func f)
        {
            using result_type = typename std::invoke_result_t<Func>;
            PackagedTask<result_type()> task(std::move(f));
            Future<result_type> future = task.get_future();

            if (l_LocalWorkQueue)
            {
                l_LocalWorkQueue->push(std::move(task));
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
        std::vector<std::unique_ptr<work_steal_queue>> m_Queues;
        std::vector<Thread> m_Threads;
        inline static thread_local work_steal_queue* l_LocalWorkQueue{nullptr};
        inline static thread_local uint32_t l_Index{UINT32_MAX};
    };

}
