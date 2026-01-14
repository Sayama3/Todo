//
// Created by ianpo on 12/01/2026.
//

#pragma once

#include "Todo/Core/Allocator.hpp"

namespace Todo
{

    template <typename T, typename Alloc = TAllocator<T>>
    class SharedResult
    {
    private:
        using TypePtr = T*;
        using ErrPtr = std::exception_ptr*;

        enum ValueType : uint8_t
        {
            V_None = 0,
            V_Value,
            V_Error,
        };

        struct Result
        {
            ValueType type{};

            union
            {
                TypePtr value{nullptr};
                ErrPtr error;
            };
        };

    public:
        SharedResult();
        ~SharedResult();
        SharedResult(const SharedResult& o);
        SharedResult& operator=(const SharedResult& o);
        SharedResult(SharedResult&& o) noexcept;
        SharedResult& operator=(SharedResult&& o) noexcept;
        void swap(SharedResult& o) noexcept;

    public:
        [[maybe_unused]] void set_value(const T& value);

        [[maybe_unused]] void set_value(T&& value);

        [[maybe_unused]] void set_exception(const std::exception_ptr& exception);

        [[nodiscard]] const T& get();

        void wait_ready();

        [[nodiscard]] const T& wait_and_get();

        [[nodiscard]] bool is_ready() const;

        [[nodiscard]] bool has_value() const;

        [[nodiscard]] bool has_error() const;

    private:
        void release();

        void set(T* data);

    private:
        std::atomic_uint64_t* p_Count{nullptr};
        std::atomic<Result>* p_Result{nullptr};
        std::atomic_flag* p_Ready{nullptr};
    };

    template <typename T, typename Alloc>
    SharedResult<T, Alloc>::SharedResult() : p_Count(new std::atomic_uint64_t(1)), p_Result(new std::atomic<Result>()), p_Ready(new std::atomic_flag)
    {
    }

    template <typename T, typename Alloc>
    SharedResult<T, Alloc>::~SharedResult()
    {
        release();
    }

    template <typename T, typename Alloc>
    SharedResult<T, Alloc>::SharedResult(const SharedResult& o) : p_Count(o.p_Count), p_Result(o.p_Result), p_Ready(o.p_Ready)
    {
        p_Count->fetch_add(1, std::memory_order_release);
    }

    template <typename T, typename Alloc>
    SharedResult<T, Alloc>& SharedResult<T, Alloc>::operator=(const SharedResult& o)
    {
        release();

        p_Count = o.p_Count;
        p_Result = o.p_Result;
        p_Ready = o.p_Ready;

        p_Count->fetch_add(1, std::memory_order_release);
        return *this;
    }

    template <typename T, typename Alloc>
    SharedResult<T, Alloc>::SharedResult(SharedResult&& o) noexcept
    {
        swap(o);
    }

    template <typename T, typename Alloc>
    SharedResult<T, Alloc>& SharedResult<T, Alloc>::operator=(SharedResult&& o) noexcept
    {
        swap(o);
        return *this;
    }

    template <typename T, typename Alloc>
    void SharedResult<T, Alloc>::swap(SharedResult& o) noexcept
    {
        std::swap(p_Result, o.p_Result);
        std::swap(p_Count, o.p_Count);
        std::swap(p_Ready, o.p_Ready);
    }

    template <typename T, typename Alloc>
    void SharedResult<T, Alloc>::set_value(const T& value)
    {
        Alloc allocator;
        T* alloc = allocator.allocate(1);
        std::construct_at(alloc, value);
        set(alloc);
    }

    template <typename T, typename Alloc>
    void SharedResult<T, Alloc>::set_value(T&& value)
    {
        Alloc allocator;
        T* alloc = allocator.allocate(1);
        std::construct_at(alloc, std::move(value));
        set(alloc);
    }

    template <typename T, typename Alloc>
    void SharedResult<T, Alloc>::set_exception(const std::exception_ptr& exception)
    {
        Result err_res {
            ValueType::V_Error,
            {new std::exception_ptr(exception)
            }
        };

        Result expected{};
        if (!p_Result->compare_exchange_strong(expected, err_res, std::memory_order_relaxed, std::memory_order_relaxed))
        {
            delete err_res.error;
            throw std::runtime_error("value already set.");
        }
        else
        {
            p_Ready->test_and_set(std::memory_order_release);
            p_Ready->notify_all();
        }
    }

    template <typename T, typename Alloc>
    const T& SharedResult<T, Alloc>::get()
    {
        Result result = p_Result->load(std::memory_order_acquire);
        if (!result.type || !result.value)
        {
            throw std::runtime_error("Value not set.");
        }

        switch (result.type)
        {
        case V_Value:
            return *result.value;
            break;
        case V_Error:
            if (*result.error)
                std::rethrow_exception(*result.error);
            break;
        default:
            throw std::runtime_error("Value not set.");
            break;
        }

        // Ideally I would use : std::unreachable();
        return T();
    }

    template <typename T, typename Alloc>
    void SharedResult<T, Alloc>::wait_ready()
    {
        if (p_Ready->test(std::memory_order_acquire))
        {
            return;
        }
        p_Ready->wait(false, std::memory_order_acquire);
        return;
    }

    template <typename T, typename Alloc>
    const T& SharedResult<T, Alloc>::wait_and_get()
    {
        wait_ready();

        if (p_Result->type != V_None)
        {
            switch (p_Result->type)
            {
            case V_Value:
                return *p_Result->value;
            case V_Error:
                std::rethrow_exception(*p_Result->error);
            default:
                break;
            }
        }
    }

    template <typename T, typename Alloc>
    bool SharedResult<T, Alloc>::is_ready() const
    {
        return p_Ready->test(std::memory_order_acquire);
    }

    template <typename T, typename Alloc>
    bool SharedResult<T, Alloc>::has_value() const
    {
        return p_Result->load(std::memory_order_acquire).type == V_Value;
    }

    template <typename T, typename Alloc>
    bool SharedResult<T, Alloc>::has_error() const
    {
        return p_Result->load(std::memory_order_acquire).type == V_Error;
    }

    template <typename T, typename Alloc>
    void SharedResult<T, Alloc>::set(T* data)
    {

        Result result{V_Value, data};
        Result current{};

        if (!p_Result->compare_exchange_strong(current, result, std::memory_order_relaxed, std::memory_order_relaxed))
        {
            std::destroy_at(data);
            Alloc allocator;
            allocator.deallocate(data);

            throw std::runtime_error("value already set.");
        }
        else
        {
            p_Ready->test_and_set(std::memory_order_release);
            p_Ready->notify_all();
        }
    }

    template <typename T, typename Alloc>
    void SharedResult<T, Alloc>::release()
    {
        if (!p_Count)
        {
            return;
        }
        TODO_ASSERT(p_Result, "No valid pointer for p_Result");

        const auto c = p_Count->fetch_sub(1, std::memory_order_acquire);
        if (c == 1)
        {
            Result res = p_Result->exchange(Result{});
            if (res.value)
            {
                switch (res.type)
                {
                case V_Value:
                    {
                        std::destroy_at(res.value);
                        Alloc allocator;
                        allocator.deallocate(res.value);
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

            delete p_Result;
            delete p_Count;
            delete p_Ready;
        }

        p_Result = nullptr;
        p_Count  = nullptr;
        p_Ready  = nullptr;
    }
}
