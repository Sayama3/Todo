//
// Created by ianpo on 12/01/2026.
//

#pragma once
#include "Future.hpp"
#include "SharedResult.hpp"

namespace Todo
{
    template<class R>
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

        Todo::Future<R> get_future();

        void set_value(const R& value);
        void set_value(R&& value);
        void set_exception( std::exception_ptr p );
    private:
        SharedResult<R> result;
    };

    template <class R>
    Promise<R>::Promise() = default;

    template <class R>
    Promise<R>::~Promise() = default;

    template <class R>
    Promise<R>::Promise(Promise&& o) noexcept
    {
        swap(o);
    }

    template <class R>
    Promise<R>& Promise<R>::operator=(Promise&& o) noexcept
    {
        swap(o);
        return *this;
    }

    template <class R>
    void Promise<R>::swap(Promise& o) noexcept
    {
        std::swap(result, o.result);
    }

    template <class R>
    Future<R> Promise<R>::get_future()
    {
        return Future<T>{result};
    }

    template <class R>
    void Promise<R>::set_value(const R& value)
    {
        result.set_value(value);
    }

    template <class R>
    void Promise<R>::set_value(R&& value)
    {
        result.set_value(std::move(value));
    }

    template <class R>
    void Promise<R>::set_exception(std::exception_ptr p)
    {
        result.set_exception(p);
    }
}
