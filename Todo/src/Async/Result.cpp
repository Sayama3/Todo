//
// Created by ianpo on 20/01/2026.
//

#include "Todo/Async/Result.hpp"

namespace Todo
{
    

     Result<void>::Result() noexcept = default;

     Result<void>::~Result()
    {
        release();
    }

     void Result<void>::set_value()
    {
        set();
    }

     void Result<void>::set_exception(const std::exception_ptr& exception)
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

     void Result<void>::get_ref()
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

     void Result<void>::get()
    {
        ResultType null_result{};
        ResultType current = m_Result.load(std::memory_order_relaxed);
        while (m_Result.compare_exchange_weak(current, null_result, std::memory_order_acquire,
                                               std::memory_order_relaxed)) {}
        if (m_NotValid.test_and_set(std::memory_order_release))
        {
            if (current.value != V_None)
            {
                // Ayo, not supposed to get there... I get we will get a
                throw std::runtime_error("The ResultType is not NONE even though the structure is in invalid mode.");
            }

            // if true, another thread already set the ResultType to invalid.
            throw std::future_error(std::future_errc::future_already_retrieved);
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
        throw std::future_error(std::future_errc::no_state);
        return;
    }

     void Result<void>::wait_ready()
    {
        if (m_Ready.test(std::memory_order_acquire))
        {
            return;
        }
        m_Ready.wait(false, std::memory_order_acquire);
    }

     void Result<void>::wait_and_get_ref()
    {
        wait_ready();

        return get_ref();
    }

     void Result<void>::wait_and_get()
    {
        wait_ready();

        return get();
    }

     bool Result<void>::is_ready() const
    {
        return m_Ready.test(std::memory_order_acquire);
    }

     bool Result<void>::is_valid() const
    {
        return m_NotValid.test(std::memory_order_acquire);
    }

     bool Result<void>::has_value() const
    {
        return m_Result.load(std::memory_order_acquire).type == V_Value;
    }

     bool Result<void>::has_error() const
    {
        return m_Result.load(std::memory_order_acquire).type == V_Error;
    }

     void Result<void>::set()
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

     void Result<void>::release()
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