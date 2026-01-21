//
// Created by ianpo on 20/01/2026.
//

#include "Todo/Async/SharedFuture.hpp"

namespace Todo
{
    SharedFuture<void>::SharedFuture() : m_Result(MakeSharedVoidResult())
    {
    }

    SharedFuture<void>::~SharedFuture() = default;

    SharedFuture<void>::SharedFuture(const SharedFuture& o) = default;

    SharedFuture<void>& SharedFuture<void>::operator=(const SharedFuture& o) = default;

    SharedFuture<void>::SharedFuture(SharedFuture&& o) noexcept : m_Result(MakeSharedVoidResult())
    {
        swap(o);
    }

    SharedFuture<void>& SharedFuture<void>::operator=(SharedFuture&& o) noexcept
    {
        swap(o);
        return *this;
    }

    void SharedFuture<void>::swap(SharedFuture& other) noexcept
    {
        std::swap(m_Result, other.m_Result);
    }

    bool SharedFuture<void>::is_ready() const
    {
        return m_Result->is_ready();
    }

    bool SharedFuture<void>::is_valid() const
    {
        return m_Result->is_valid();
    }

    void SharedFuture<void>::wait() const
    {
        m_Result->wait_ready();
    }

    void SharedFuture<void>::get() const
    {
        if (!m_Result->is_valid())
        {
            throw std::future_error(std::future_errc::future_already_retrieved);
        }

        m_Result->wait_and_get_ref();
    }

    SharedFuture<void>::SharedFuture(SharedResult<void> result) : m_Result(std::move(result))
    {
    }
}
