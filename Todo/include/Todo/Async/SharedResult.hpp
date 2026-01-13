//
// Created by ianpo on 12/01/2026.
//

#pragma once

#include "Todo/Core/Allocator.hpp"
#include "Todo/Core/Concepts.hpp"
#include "Todo/Multithreading/Mutex.hpp"
#include "Todo/Multithreading/TMutex.hpp"

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
        SharedResult() = default;
        ~SharedResult();

    public:
        [[maybe_unused]] void set_value(const T& value)
        {
            Alloc allocator;
            T* alloc = allocator.allocate(1);
            std::construct_at(alloc, value);
            set(alloc);
        }

        [[maybe_unused]] void set_value(T&& value)
        {
            Alloc allocator;
            T* alloc = allocator.allocate(1);
            std::construct_at(alloc, std::move(value));
            set(alloc);
        }

        T get()
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

        [[nodiscard]] bool is_ready() const
        {
            return p_Result->load(std::memory_order_acquire).type != V_None;
        }

        [[nodiscard]] bool has_value() const
        {
            return p_Result->load(std::memory_order_acquire).type == V_Value;
        }

        [[nodiscard]] bool has_error() const
        {
            return p_Result->load(std::memory_order_acquire).type == V_Error;
        }

    private:
        void set(T* data)
        {

            Result result{V_Value, data};
            Result current{};

            if (!p_Result->compare_exchange_strong(current, result, std::memory_order_release, std::memory_order_relaxed))
            {
                std::destroy_at(data);
                Alloc allocator;
                allocator.deallocate(data);

                throw std::runtime_error("value already set.");
            }
        }
    private:
        std::atomic_uint64_t count{};
        std::atomic<Result>* p_Result{nullptr};
    };

    template <typename T, typename Alloc>
    SharedResult<T, Alloc>::~SharedResult()
    {
        const auto c = count.fetch_sub(1, std::memory_order_relaxed);
        if (c == 1)
        {
            Result res = p_Result->exchange(Result{});
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
    }

    template <>
    class SharedResult<void>
    {
        ~SharedResult() = default;
    };
}
