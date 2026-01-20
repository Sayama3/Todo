//
// Created by ianpo on 12/01/2026.
//

#pragma once
#include "Future.hpp"
#include "SharedResult.hpp"

namespace Todo
{
    template<class R, typename Alloc = TAllocator<R>>
    class Promise
    {
    public:
        Promise();
        ~Promise();
        Promise(const Promise&) = delete;
        Promise& operator=(const Promise&) = delete;
        Promise(Promise&& o) noexcept;
        Promise& operator=(Promise&& o) noexcept;
        void swap(Promise& o) noexcept;

    public:

        Todo::Future<R, Alloc> get_future();

        void set_value(const R& value);
        void set_value(R&& value);
        void set_exception( std::exception_ptr p );

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] bool is_ready() const;

    private:
        SharedResult<R, Alloc> result;
    };

    template <class R, typename Alloc>
    Promise<R, Alloc>::Promise() = default;

    template <class R, typename Alloc>
    Promise<R, Alloc>::~Promise() = default;

    template <class R, typename Alloc>
    Promise<R, Alloc>::Promise(Promise&& o) noexcept
    {
        swap(o);
    }

    template <class R, typename Alloc>
    Promise<R, Alloc>& Promise<R, Alloc>::operator=(Promise&& o) noexcept
    {
        swap(o);
        return *this;
    }

    template <class R, typename Alloc>
    void Promise<R, Alloc>::swap(Promise& o) noexcept
    {
        std::swap(result, o.result);
    }

    template <class R, typename Alloc>
    Future<R, Alloc> Promise<R, Alloc>::get_future()
    {
        return Future<R, Alloc>{result};
    }

    template <class R, typename Alloc>
    void Promise<R, Alloc>::set_value(const R& value)
    {
        result.set_value(value);
    }

    template <class R, typename Alloc>
    void Promise<R, Alloc>::set_value(R&& value)
    {
        result.set_value(std::move(value));
    }

    template <class R, typename Alloc>
    void Promise<R, Alloc>::set_exception(std::exception_ptr p)
    {
        result.set_exception(p);
    }

    template <class R, typename Alloc>
    bool Promise<R, Alloc>::is_valid() const
    {
        return result.is_valid();
    }

    template <class R, typename Alloc>
    bool Promise<R, Alloc>::is_ready() const
    {
        return result.is_ready();
    }
}
