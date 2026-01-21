//
// Created by ianpo on 20/01/2026.
//

#include "Todo/Async/Future.hpp"

namespace Todo
{
    Future<void>::Future() : m_Result(MakeSharedVoidResult())
    {
    }

    Future<void>::~Future() = default;

    Future<void>::Future(Future&& o) noexcept : m_Result(MakeSharedVoidResult())
    {
        swap(o);
    }

    Future<void>& Future<void>::operator=(Future&& o) noexcept
    {
        swap(o);
        return *this;
    }

    void Future<void>::swap(Future& o) noexcept
    {
        std::swap(m_Result, o.m_Result);
    }

    Future<void>::Future(SharedResult<void> result)
    {
        m_Result = std::move(result);
    }

    SharedFuture<void> Future<void>::share()
    {
        return SharedFuture<void>(std::move(m_Result));
    }

    bool Future<void>::is_ready() const
    {
        return m_Result->is_ready();
    }

    bool Future<void>::is_valid() const
    {
        return m_Result->is_valid();
    }

    void Future<void>::wait() const
    {
        m_Result->wait_ready();
    }

    void Future<void>::get() const
    {
        m_Result->wait_and_get();
    }
}
