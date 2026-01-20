//
// Created by ianpo on 12/01/2026.
//

#pragma once

#include "SharedResult.hpp"

namespace Todo
{
    template <>
    class SharedResult<void>
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

        struct Result
        {
            ValueType type{};

            union
            {
                TypePtr value{0u};
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
        [[maybe_unused]] void set_value();

        [[maybe_unused]] void set_exception(const std::exception_ptr& exception);

        void get_ref();

        void get();

        void wait_ready();

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
        std::atomic_uint64_t* p_Count{nullptr};
        std::atomic<Result>* p_Result{nullptr};
        std::atomic_flag* p_Ready{nullptr};
        std::atomic_flag* p_NotValid{nullptr};
    };

    inline SharedResult<void>::SharedResult() : p_Count(new std::atomic_uint64_t(1)), p_Result(new std::atomic<Result>()),
                                             p_Ready(new std::atomic_flag), p_NotValid(new std::atomic_flag)
    {
    }

    inline SharedResult<void>::~SharedResult()
    {
        release();
    }

    inline SharedResult<void>::SharedResult(const SharedResult& o) : p_Count(o.p_Count), p_Result(o.p_Result),
                                                                  p_Ready(o.p_Ready), p_NotValid(o.p_NotValid)
    {
        p_Count->fetch_add(1, std::memory_order_release);
    }

    inline SharedResult<void>& SharedResult<void>::operator=(const SharedResult& o)
    {
        release();

        p_Count = o.p_Count;
        p_Result = o.p_Result;
        p_Ready = o.p_Ready;
        p_NotValid = o.p_NotValid;

        p_Count->fetch_add(1, std::memory_order_release);
        return *this;
    }

    inline SharedResult<void>::SharedResult(SharedResult&& o) noexcept
    {
        swap(o);
    }

    inline SharedResult<void>& SharedResult<void>::operator=(SharedResult&& o) noexcept
    {
        swap(o);
        return *this;
    }

    inline void SharedResult<void>::swap(SharedResult& o) noexcept
    {
        std::swap(p_Result, o.p_Result);
        std::swap(p_Count, o.p_Count);
        std::swap(p_Ready, o.p_Ready);
        std::swap(p_NotValid, o.p_NotValid);
    }

    inline void SharedResult<void>::set_value()
    {
        set();
    }

    inline void SharedResult<void>::set_exception(const std::exception_ptr& exception)
    {
        Result err_res{
            ValueType::V_Error,
            {
                .error = new std::exception_ptr(exception)
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
            p_NotValid->clear(std::memory_order_relaxed);
            p_Ready->test_and_set(std::memory_order_release);
            p_Ready->notify_all();
        }
    }

    inline void SharedResult<void>::get_ref()
    {
        Result result = p_Result->load(std::memory_order_acquire);
        if (!result.type || !result.value)
        {
            throw std::runtime_error("Value not set.");
        }

        switch (result.type)
        {
        case V_Value:
            return;
            break;
        case V_Error:
            if (*result.error)
                std::rethrow_exception(*result.error);
            break;
        default:
            break;
        }
        TODO_ERR("Value not set.");
        return;
    }

    inline void SharedResult<void>::get()
    {
        Result null_result{};
        Result current = p_Result->load(std::memory_order_relaxed);
        while (p_Result->compare_exchange_weak(current, null_result, std::memory_order_acquire,
                                               std::memory_order_relaxed));
        if (p_NotValid->test_and_set(std::memory_order_release))
        {
            if (current.value != V_None)
            {
                // Ayo, not supposed to get there... I get we will get a
                throw std::runtime_error("The result is not NONE even though the structure is in invalid mode.");
            }

            // if true, another thread already set the result to invalid.
            return;
        }

        if (current.type != V_None)
        {
            switch (current.type)
            {
            case V_Value:
                {
                    return;
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
        TODO_ERR("No value in result.");
        return;
    }

    inline void SharedResult<void>::wait_ready()
    {
        if (p_Ready->test(std::memory_order_acquire))
        {
            return;
        }
        p_Ready->wait(false, std::memory_order_acquire);
    }

    inline void SharedResult<void>::wait_and_get_ref()
    {
        wait_ready();

        return get_ref();
    }

    inline void SharedResult<void>::wait_and_get()
    {
        wait_ready();

        return get();
    }

    inline bool SharedResult<void>::is_ready() const
    {
        return p_Ready->test(std::memory_order_acquire);
    }

    inline bool SharedResult<void>::is_valid() const
    {
        return p_NotValid->test(std::memory_order_acquire);
    }

    inline bool SharedResult<void>::has_value() const
    {
        return p_Result->load(std::memory_order_acquire).type == V_Value;
    }

    inline bool SharedResult<void>::has_error() const
    {
        return p_Result->load(std::memory_order_acquire).type == V_Error;
    }

    inline void SharedResult<void>::set()
    {
        Result result{V_Value, {1}};
        Result current{};

        if (!p_Result->compare_exchange_strong(current, result, std::memory_order_relaxed, std::memory_order_relaxed))
        {
            throw std::runtime_error("value already set.");
        }
        else
        {
            p_NotValid->clear(std::memory_order_relaxed);
            p_Ready->test_and_set(std::memory_order_release);
            p_Ready->notify_all();
        }
    }

    inline void SharedResult<void>::release()
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
            delete p_NotValid;
        }

        p_Result = nullptr;
        p_Count = nullptr;
        p_Ready = nullptr;
        p_NotValid = nullptr;
    }
}
