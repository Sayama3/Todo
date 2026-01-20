//
// Created by ianpo on 12/01/2026.
//

#pragma once

#include "Result.hpp"

namespace Todo
{
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

    inline Result<void>::Result() noexcept = default;

    inline Result<void>::~Result()
    {
        release();
    }

    inline void Result<void>::set_value()
    {
        set();
    }

    inline void Result<void>::set_exception(const std::exception_ptr& exception)
    {
        ResultType err_res{
            ValueType::V_Error,
            {
                .error = new std::exception_ptr(exception)
            }
        };

        ResultType expected{};
        if (!m_Result.compare_exchange_strong(expected, err_res, std::memory_order_relaxed, std::memory_order_relaxed))
        {
            delete err_res.error;
            throw std::runtime_error("value already set.");
        }
        else
        {
            m_NotValid.clear(std::memory_order_relaxed);
            m_Ready.test_and_set(std::memory_order_release);
            m_Ready.notify_all();
        }
    }

    inline void Result<void>::get_ref()
    {
        ResultType ResultType = m_Result.load(std::memory_order_acquire);
        if (!ResultType.type || !ResultType.value)
        {
            throw std::runtime_error("Value not set.");
        }

        switch (ResultType.type)
        {
        case V_Value:
            return;
            break;
        case V_Error:
            if (*ResultType.error)
                std::rethrow_exception(*ResultType.error);
            break;
        default:
            break;
        }
        TODO_ERR("Value not set.");
        return;
    }

    inline void Result<void>::get()
    {
        ResultType null_result{};
        ResultType current = m_Result.load(std::memory_order_relaxed);
        while (m_Result.compare_exchange_weak(current, null_result, std::memory_order_acquire,
                                               std::memory_order_relaxed));
        if (m_NotValid.test_and_set(std::memory_order_release))
        {
            if (current.value != V_None)
            {
                // Ayo, not supposed to get there... I get we will get a
                throw std::runtime_error("The ResultType is not NONE even though the structure is in invalid mode.");
            }

            // if true, another thread already set the ResultType to invalid.
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
        TODO_ERR("No value in ResultType.");
        return;
    }

    inline void Result<void>::wait_ready()
    {
        if (m_Ready.test(std::memory_order_acquire))
        {
            return;
        }
        m_Ready.wait(false, std::memory_order_acquire);
    }

    inline void Result<void>::wait_and_get_ref()
    {
        wait_ready();

        return get_ref();
    }

    inline void Result<void>::wait_and_get()
    {
        wait_ready();

        return get();
    }

    inline bool Result<void>::is_ready() const
    {
        return m_Ready.test(std::memory_order_acquire);
    }

    inline bool Result<void>::is_valid() const
    {
        return m_NotValid.test(std::memory_order_acquire);
    }

    inline bool Result<void>::has_value() const
    {
        return m_Result.load(std::memory_order_acquire).type == V_Value;
    }

    inline bool Result<void>::has_error() const
    {
        return m_Result.load(std::memory_order_acquire).type == V_Error;
    }

    inline void Result<void>::set()
    {
        ResultType resultType{V_Value, {1}};
        ResultType current{};

        if (!m_Result.compare_exchange_strong(current, resultType, std::memory_order_relaxed, std::memory_order_relaxed))
        {
            throw std::runtime_error("value already set.");
        }
        else
        {
            m_NotValid.clear(std::memory_order_relaxed);
            m_Ready.test_and_set(std::memory_order_release);
            m_Ready.notify_all();
        }
    }

    inline void Result<void>::release()
    {
        ResultType res = m_Result.exchange(ResultType{});
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
    }
}
