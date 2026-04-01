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

#include "Todo/Core/Concepts.hpp"
#include "Todo/Core/Allocator.hpp"
#include "Todo/Core/Helper.hpp"

namespace Todo
{
    template <class>
    class Result;

    template <typename T>
    class Result
    {
    private:
        using TypePtr = T*;
        using ErrPtr = std::exception_ptr*;
        using Alloc = TAllocator<T>;

        enum ValueType : uint8_t
        {
            V_None = 0,
            V_Value,
            V_Error,
        };

        struct ResultType
        {
            ResultType() noexcept = default;
            explicit ResultType(ErrPtr pExcept) noexcept : type(V_Error), error(pExcept) {}
            explicit ResultType(TypePtr pValue) noexcept : type(V_Value), value(pValue) {}
            ~ResultType() = default;
            ResultType(const ResultType&) noexcept = default;
            ResultType& operator=(const ResultType&) noexcept = default;
            ResultType(ResultType&) noexcept = default;
            ResultType& operator=(ResultType&) noexcept = default;

            ValueType type{};

            union
            {
                TypePtr value{nullptr};
                ErrPtr error;
            };
        };

    public:
        Result() noexcept;
        ~Result();
        Result(const Result& o) = delete;
        Result& operator=(const Result& o) = delete;
        Result(Result&& o) noexcept = delete;
        Result& operator=(Result&& o) noexcept = delete;
        void swap(Result& o) noexcept = delete;

    public:
        [[maybe_unused]] void set_value(const T& value);

        [[maybe_unused]] void set_value(T&& value);

        [[maybe_unused]] void set_exception(const std::exception_ptr& exception);

        [[nodiscard]] const T* get_ref();

        [[nodiscard]] std::optional<T> get();

        void wait_ready();

        template <typename Rep, typename Period>
        std::future_status wait_for(const std::chrono::duration<Rep, Period>& duration);

        template <StdClock Clock, typename Duration>
        std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& time_point);

        [[nodiscard]] const T* wait_and_get_ref();

        [[nodiscard]] std::optional<T> wait_and_get();

        [[nodiscard]] bool is_ready() const;

        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] bool has_value() const;

        [[nodiscard]] bool has_error() const;

    private:
        void release();

        void set(T* data);

    private:
        std::atomic<ResultType> m_Result{};
        std::atomic_flag m_Ready{};
        std::atomic_flag m_NotValid{};
    };

    template <typename T>
    Result<T>::Result() noexcept = default;

    template <typename T>
    Result<T>::~Result()
    {
        release();
    }

    template <typename T>
    void Result<T>::set_value(const T& value)
    {
        Alloc allocator;
        T* alloc = allocator.allocate(1);
        std::construct_at(alloc, value);
        set(alloc);
    }

    template <typename T>
    void Result<T>::set_value(T&& value)
    {
        Alloc allocator;
        T* alloc = allocator.allocate(1);
        std::construct_at(alloc, std::move(value));
        set(alloc);
    }

    template <typename T>
    void Result<T>::set_exception(const std::exception_ptr& exception)
    {
        ResultType err_res{new std::exception_ptr(exception)};

        ResultType expected{};
        if (!m_Result.compare_exchange_strong(expected, err_res, std::memory_order_relaxed, std::memory_order_relaxed))
        {
            delete err_res.error;
            throw std::future_error(std::future_errc::promise_already_satisfied);
        }
        else
        {
            m_NotValid.clear(std::memory_order_relaxed);
            m_Ready.test_and_set(std::memory_order_release);
            m_Ready.notify_all();
        }
    }

    template <typename T>
    const T* Result<T>::get_ref()
    {
        if (!m_Ready.test(std::memory_order_acquire))
        {
            return nullptr;
        }

        if (m_NotValid.test(std::memory_order_acquire))
        {
            // if true, another thread already set the ResultType to invalid.
            return nullptr;
        }

        ResultType resultType = m_Result.load(std::memory_order_relaxed);
        if (!resultType.type || !resultType.value)
        {
            throw std::runtime_error("Value not set.");
        }

        switch (resultType.type)
        {
        case V_Value:
            return resultType.value;
            break;
        case V_Error:
            if (*resultType.error)
                std::rethrow_exception(*resultType.error);
            break;
        default:
            break;
        }

        TODO_ERR("Value not set.");
        return nullptr;
    }

    template <typename T>
    std::optional<T> Result<T>::get()
    {
        if (!m_Ready.test(std::memory_order_acquire))
        {
            return std::nullopt;
        }

        if (m_NotValid.test_and_set(std::memory_order_acq_rel))
        {
            // if true, another thread already set the ResultType to invalid.
            return std::nullopt;
        }

        const ResultType current = m_Result.exchange(ResultType{}, std::memory_order_relaxed);

        if (current.type != V_None)
        {
            switch (current.type)
            {
            case V_Value:
                {
                    T value = std::move(*current.value);
                    {
                        Alloc alloc;
                        std::destroy_at(current.value);
                        alloc.deallocate(current.value, 1);
                    }
                    return std::move(value);
                }
                break;
            case V_Error:
                {
                    std::exception_ptr err = *current.error;
                    delete current.error;
                    std::rethrow_exception(err);
                    break;
                }
            default:
                break;
            }
        }

        TODO_ERR("No value in ResultType.");
        return std::nullopt;
    }

    template <typename T>
    void Result<T>::wait_ready()
    {
        if (m_Ready.test(std::memory_order_acquire))
        {
            return;
        }
        m_Ready.wait(false, std::memory_order_acquire);
    }

    template <typename T>
    template <typename Rep, typename Period>
    std::future_status Result<T>::wait_for(const std::chrono::duration<Rep, Period>& duration)
    {
        using Duration = std::chrono::duration<uint64_t, std::nano>;
        using Clock = std::chrono::steady_clock; // Recommended by the standard
        using TimePoint = std::chrono::time_point<Clock, Duration>;
        if (m_Ready.test(std::memory_order_acquire))
        {
            return std::future_status::ready;
        }

        //TODO: implement deferred thingy ???

        // Only doing the yielding and busy waiting if there is any duration at all.
        if (duration > duration.zero())
        {
            // Busy wait loop to be as close to the duration as possible while being able to early exit
            const TimePoint start = Todo::ClockNow<Clock, Duration>();
            const TimePoint end = start + duration;
            do
            {
                std::this_thread::yield();
            }
            while (Todo::ClockNow<Clock, Duration>() < end && !m_Ready.test(std::memory_order_relaxed));

            if (m_Ready.test(std::memory_order_acquire))
            {
                return std::future_status::ready;
            }
        }

        return std::future_status::timeout;
    }

    template <typename T>
    template <StdClock Clock, typename Duration>
    std::future_status Result<T>::wait_until(const std::chrono::time_point<Clock, Duration>& time_point)
    {
        using RealDuration = std::chrono::duration<uint64_t, std::nano>;
        using TimePoint = std::chrono::time_point<Clock, RealDuration>;
        const TimePoint desired_time_point = TimePoint(std::chrono::duration_cast<RealDuration>(time_point.time_since_epoch()));

        // Clock already set
        // Duration already set.

        if (m_Ready.test(std::memory_order_acquire))
        {
            return std::future_status::ready;
        }

        //TODO: implement deferred thingy ???

        // Busy wait loop to be as close to the time_point as possible while being able to early exit
        while (Todo::ClockNow<Clock, RealDuration>() < desired_time_point && !m_Ready.test(std::memory_order_relaxed))
        {
            std::this_thread::yield();
        }

        if (m_Ready.test(std::memory_order_acquire))
        {
            return std::future_status::ready;
        }

        return std::future_status::timeout;
    }

    template <typename T>
    const T* Result<T>::wait_and_get_ref()
    {
        wait_ready();

        return get_ref();
    }

    template <typename T>
    std::optional<T> Result<T>::wait_and_get()
    {
        wait_ready();

        return get();
    }

    template <typename T>
    bool Result<T>::is_ready() const
    {
        return m_Ready.test(std::memory_order_acquire);
    }

    template <typename T>
    bool Result<T>::is_valid() const
    {
        return !m_NotValid.test(std::memory_order_acquire);
    }

    template <typename T>
    bool Result<T>::has_value() const
    {
        return m_Result.load(std::memory_order_acquire).type == V_Value;
    }

    template <typename T>
    bool Result<T>::has_error() const
    {
        return m_Result.load(std::memory_order_acquire).type == V_Error;
    }

    template <typename T>
    void Result<T>::set(T* data)
    {
        ResultType resultType{data};
        ResultType current{};

        if (!m_Result.compare_exchange_strong(current, resultType, std::memory_order_relaxed,
                                              std::memory_order_relaxed))
        {
            std::destroy_at(data);
            Alloc allocator;
            allocator.deallocate(data, 1);

            throw std::runtime_error("value already set.");
        }
        else
        {
            m_NotValid.clear(std::memory_order_relaxed);
            m_Ready.test_and_set(std::memory_order_release);
            m_Ready.notify_all();
        }
    }

    template <typename T>
    void Result<T>::release()
    {
        ResultType res = m_Result.exchange(ResultType{}, std::memory_order_relaxed);
        if (res.value)
        {
            switch (res.type)
            {
            case V_Value:
                {
                    std::destroy_at(res.value);
                    Alloc allocator;
                    allocator.deallocate(res.value, 1);
                    break;
                }
            case V_Error:
                {
                    delete res.error;
                    break;
                }
            default:
                {
                    break;
                }
            }
        }
    }

    // Void Specialization

    template <>
    class Result<void>
    {
    private:
        using TypePtr = std::uintptr_t;
        using ErrPtr = std::exception_ptr*;

        enum ValueType : uint8_t
        {
            V_None = 0,
            V_Value,
            V_Error,
        };

        struct ResultType
        {
            ResultType() noexcept = default;
            explicit ResultType(ErrPtr pExcept) noexcept : type(V_Error), error(pExcept) {}
            explicit ResultType(TypePtr pValue) noexcept : type(V_Value), value(pValue) {}
            ~ResultType() = default;
            ResultType(const ResultType&) noexcept = default;
            ResultType& operator=(const ResultType&) noexcept = default;
            ResultType(ResultType&) noexcept = default;
            ResultType& operator=(ResultType&) noexcept = default;

            ValueType type{};

            union
            {
                TypePtr value{0u};
                ErrPtr error;
            };
        };

    public:
        Result() noexcept;
        ~Result();
        Result(const Result& o) = delete;
        Result& operator=(const Result& o) = delete;
        Result(Result&& o) noexcept = delete;
        Result& operator=(Result&& o) noexcept = delete;
        void swap(Result& o) noexcept = delete;

    public:
        [[maybe_unused]] void set_value();

        [[maybe_unused]] void set_exception(const std::exception_ptr& exception);

        void get_ref();

        void get();

        void wait_ready();

        template <typename Rep, typename Period>
        std::future_status wait_for(const std::chrono::duration<Rep, Period>& duration);

        template <StdClock Clock, typename Duration>
        std::future_status wait_until(const std::chrono::time_point<Clock, Duration>& time_point);

        void wait_and_get_ref();

        void wait_and_get();

        [[nodiscard]] bool is_ready() const;

        [[nodiscard]] bool is_valid() const;

        [[nodiscard]] bool has_value() const;

        [[nodiscard]] bool has_error() const;

    private:
        void release();

        void set();

    private:
        std::atomic<ResultType> m_Result{};
        std::atomic_flag m_Ready{};
        std::atomic_flag m_NotValid{};
    };


    template <typename Rep, typename Period>
    std::future_status Result<void>::wait_for(const std::chrono::duration<Rep, Period>& duration)
    {
        using Duration = std::chrono::duration<uint64_t, std::nano>;
        using Clock = std::chrono::steady_clock; // Recommended by the standard
        using TimePoint = std::chrono::time_point<Clock, Duration>;
        if (m_Ready.test(std::memory_order_acquire))
        {
            return std::future_status::ready;
        }

        //TODO: implement deferred thingy ???

        // Only doing the yielding and busy waiting if there is any duration at all.
        if (duration > duration.zero())
        {
            // Busy wait loop to be as close to the duration as possible while being able to early exit
            const TimePoint start = Todo::ClockNow<Clock, Duration>();
            const TimePoint end = start + duration;
            do
            {
                std::this_thread::yield();
            }
            while (Todo::ClockNow<Clock, Duration>() < end && !m_Ready.test(std::memory_order_relaxed));

            if (m_Ready.test(std::memory_order_acquire))
            {
                return std::future_status::ready;
            }
        }

        return std::future_status::timeout;
    }

    template <StdClock Clock, typename Duration>
    std::future_status Result<void>::wait_until(const std::chrono::time_point<Clock, Duration>& time_point)
    {
        using RealDuration = std::chrono::duration<uint64_t, std::nano>;
        using TimePoint = std::chrono::time_point<Clock, RealDuration>;
        const TimePoint desired_time_point = TimePoint(std::chrono::duration_cast<RealDuration>(time_point.time_since_epoch()));

        // Clock already set
        // Duration already set.

        if (m_Ready.test(std::memory_order_acquire))
        {
            return std::future_status::ready;
        }

        //TODO: implement deferred thingy ???

        // Busy wait loop to be as close to the time_point as possible while being able to early exit
        while (Todo::ClockNow<Clock, RealDuration>() < desired_time_point && !m_Ready.test(std::memory_order_relaxed))
        {
            std::this_thread::yield();
        }

        if (m_Ready.test(std::memory_order_acquire))
        {
            return std::future_status::ready;
        }

        return std::future_status::timeout;
    }
}
