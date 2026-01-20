//
// Created by ianpo on 20/01/2026.
//

#include "Todo/Async/Promise.hpp"

namespace Todo
{
    Promise<void>::Promise() : m_Result(MakeSharedVoidResult())
    {
    }

    Promise<void>::~Promise() = default;

    Promise<void>::Promise(Promise&& o) noexcept : m_Result(MakeSharedVoidResult())
    {
        swap(o);
    }

    Promise<void>& Promise<void>::operator=(Promise&& o) noexcept
    {
        swap(o);
        return *this;
    }

    void Promise<void>::swap(Promise& o) noexcept
    {
        std::swap(m_Result, o.m_Result);
    }

    Future<void> Promise<void>::get_future()
    {
        return Future<void>{m_Result};
    }

    void Promise<void>::set_value()
    {
        m_Result->set_value();
    }

    void Promise<void>::set_exception(std::exception_ptr p)
    {
        m_Result->set_exception(p);
    }

    bool Promise<void>::is_valid() const
    {
        return m_Result->is_valid();
    }

    bool Promise<void>::is_ready() const
    {
        return m_Result->is_ready();
    }
}
