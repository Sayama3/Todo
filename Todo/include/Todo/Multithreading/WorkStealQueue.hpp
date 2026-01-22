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



// Modified version of the WorkStealingQueue of T.-W. Huang (cf. https://github.com/taskflow/work-stealing-queue) to support any type of T
// Lose performance for Trivial type but work for any type.
// Heavy use over the new/delete, might be worth to investigate the use of a dedicated Allocator.

#pragma once

namespace Todo
{
    /// A queue meant to be used in parallel where a single queue can push and pop at the top of the queue and the
    /// rest of the threads can steal data at the bottom of it.
    /// @tparam T Type of the data in the queue.
    template <std::movable T>
    class WorkStealQueue
    {
        using type = T;
        using type_ptr = T*;
    private:

        // Array structure that can operate on any type of T by stacking them close together and working only with pointer
        // for the atomic operations.
        struct Array
        {
            explicit Array(const int64_t count) : m_Count(count), m_Max(count - 1), m_Array(new std::atomic<type_ptr>[static_cast<size_t>(count)]) {}

            ~Array() {
                for (int64_t i = 0; i < m_Count; ++i)
                {
                    T* value = m_Array[i].exchange(nullptr, std::memory_order_seq_cst);
                    if (value)
                    {
                        delete value;
                    }
                }

                delete[] m_Array;
            }

            [[nodiscard]] int64_t capacity() const noexcept {
                return m_Count;
            }

            void push(const int64_t i, T* const ptr) noexcept
            {
                const int64_t index = i & m_Max;

                const T* result = m_Array[index].exchange(ptr, std::memory_order_relaxed);
                if (result && result != ptr)
                {
                    delete result;
                }
            }

            std::optional<T> pop(const int64_t i) noexcept
            {
                std::unique_ptr<T> current = pop_ptr(i);
                if (current)
                {
                    std::optional<T> result = std::move(*current);
                    return result;
                }
                return std::nullopt;
            }

            std::unique_ptr<T> pop_ptr(const int64_t i) noexcept
            {
                std::unique_ptr<T> result{};
                result.reset(pop_raw_ptr(i));
                return result;
            }

            T* pop_raw_ptr(const int64_t i) noexcept
            {
                const int64_t index = i & m_Max;
                T* ptr = m_Array[index].exchange(nullptr, std::memory_order_relaxed);
                return ptr;
            }

            Array* resize(const int64_t bottom, const int64_t top)
            {
                auto* ptr = new Array{2*m_Count};
                for (int64_t i = top; i != bottom; ++i)
                {
                    ptr->push(i, pop_raw_ptr(i));
                }
                return ptr;
            }

            int64_t m_Count;
            int64_t m_Max;
            std::atomic<type_ptr>* m_Array;
        };
    public:
        /**
        @brief constructs the queue with a given capacity

        @param capacity the capacity of the queue (must be power of 2)
        */
        explicit WorkStealQueue(uint32_t capacity = 1024);

        ~WorkStealQueue();

        WorkStealQueue(const WorkStealQueue&) = delete;
        WorkStealQueue& operator=(const WorkStealQueue&) = delete;

        WorkStealQueue(WorkStealQueue&&) noexcept = delete;
        WorkStealQueue& operator=(WorkStealQueue&&) noexcept = delete;
        void swap(WorkStealQueue&) noexcept = delete;

    public:
        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] size_t size() const noexcept;
        [[nodiscard]] int64_t capacity() const noexcept;

        template<typename ... Args>
        void emplace(Args&&...args);
        void push(T* data) noexcept;

        std::optional<T> pop();
        std::optional<T> steal();

        bool try_pop(T& data);
        bool try_steal(T& data);

    private:
        // avoids false sharing between m_Top and m_Bottom
        // Most implementation use 'std::hardware_destructive_interference_size == 64' so I might as well avoid the hassle
        // of the multi-compiler optimization and set it straight to 64 has recommended by GCC.
        alignas(64) std::atomic<int64_t> m_Top;
        alignas(64) std::atomic<int64_t> m_Bottom;

        std::atomic<Array*> m_Array;
        std::vector<Array*> m_Garbage;
    };

    template <std::movable T>
    WorkStealQueue<T>::WorkStealQueue(uint32_t capacity)
    {
        TODO_ASSERT(capacity && (!(capacity & (capacity-1))), "Capacity must be a power of two.");
        m_Top.store(0, std::memory_order_relaxed);
        m_Bottom.store(0, std::memory_order_relaxed);
        m_Array.store(new Array{capacity}, std::memory_order_relaxed);
        m_Garbage.reserve(32);
    }

    template <std::movable T>
    WorkStealQueue<T>::~WorkStealQueue()
    {
        for(auto a : m_Garbage) {
            delete a;
        }
        delete m_Array.load();
    }

    template <std::movable T>
    bool WorkStealQueue<T>::empty() const noexcept
    {
        int64_t b = m_Bottom.load(std::memory_order_relaxed);
        int64_t t = m_Top.load(std::memory_order_relaxed);
        return b <= t;
    }

    template <std::movable T>
    size_t WorkStealQueue<T>::size() const noexcept
    {
        int64_t b = m_Bottom.load(std::memory_order_relaxed);
        int64_t t = m_Top.load(std::memory_order_relaxed);
        return static_cast<size_t>(b >= t ? b - t : 0);
    }

    template <std::movable T>
    int64_t WorkStealQueue<T>::capacity() const noexcept
    {
        return m_Array.load(std::memory_order_relaxed)->capacity();
    }

    template <std::movable T>
    template <typename ... Args>
    void WorkStealQueue<T>::emplace(Args&&... args)
    {
        T* data = new T(std::forward<Args>(args)...);
        push(data);
    }

    template <std::movable T>
    void WorkStealQueue<T>::push(T* data) noexcept
    {
        int64_t b = m_Bottom.load(std::memory_order_relaxed);
        int64_t t = m_Top.load(std::memory_order_acquire);
        Array* a = m_Array.load(std::memory_order_relaxed);

        // queue is full
        if(a->capacity() - 1 < (b - t)) {
            Array* tmp = a->resize(b, t);
            m_Garbage.push_back(a);
            std::swap(a, tmp);
            m_Array.store(a, std::memory_order_relaxed);
        }

        a->push(b, data);
        std::atomic_thread_fence(std::memory_order_release);
        m_Bottom.store(b + 1, std::memory_order_relaxed);
    }

    template <std::movable T>
    std::optional<T> WorkStealQueue<T>::pop()
    {
        int64_t b = m_Bottom.load(std::memory_order_relaxed) - 1;
        Array* a = m_Array.load(std::memory_order_relaxed);
        m_Bottom.store(b, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        int64_t t = m_Top.load(std::memory_order_relaxed);

        std::optional<T> item;

        if(t <= b) {
            item = a->pop(b);
            if(t == b) {
                // the last item just got stolen
                if(!m_Top.compare_exchange_strong(t, t+1,
                                                 std::memory_order_seq_cst,
                                                 std::memory_order_relaxed)) {
                    item = std::nullopt;
                                                 }
                m_Bottom.store(b + 1, std::memory_order_relaxed);
            }
        }
        else {
            m_Bottom.store(b + 1, std::memory_order_relaxed);
        }

        return item;
    }

    template <std::movable T>
    std::optional<T> WorkStealQueue<T>::steal()
    {
        int64_t t = m_Top.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        int64_t b = m_Bottom.load(std::memory_order_acquire);

        std::optional<T> item;

        if(t < b) {
            Array* a = m_Array.load(std::memory_order_consume);
            item = a->pop(t);
            if(!m_Top.compare_exchange_strong(t, t+1,
                                             std::memory_order_seq_cst,
                                             std::memory_order_relaxed)) {
                return std::nullopt;
                                             }
        }

        return item;
    }

    template <std::movable T>
    bool WorkStealQueue<T>::try_pop(T& data)
    {
        std::optional<T> value = pop();
        if (value.has_value())
        {
            data = std::move(value.value());
            return true;
        }
        return false;
    }

    template <std::movable T>
    bool WorkStealQueue<T>::try_steal(T& data)
    {
        std::optional<T> value = steal();
        if (value.has_value())
        {
            data = std::move(value.value());
            return true;
        }
        return false;
    }
}
