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

#include "ThreadPool.hpp"
#include "Todo/Multithreading/Thread.hpp"

namespace Todo
{
    class JobSystem
    {
        struct DedicatedThread
        {
            std::function<void()> func;
            std::unique_ptr<Thread> thread;
        };

    public:
        JobSystem();
        JobSystem(uint32_t threadCount);
        ~JobSystem();

    public:
        JobSystem& AddDedicatedThread(std::function<void()> func, uint32_t& id);

    public:
        void Run();

    private:
        inline static uint32_t g_ID{1};
        uint32_t thread_count{0u};
        std::unordered_map<uint32_t, DedicatedThread> m_DedicatedThreads;
        std::unique_ptr<ThreadPool> m_ThreadPool;
    };
} // Todo
