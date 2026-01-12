//
// Created by ianpo on 12/01/2026.
//

#pragma once

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
        Future<R> get_future();

        void set_value(const R& value);
        void set_value(R&& value);
        void set_exception( std::exception_ptr p );
    };

    template<>
    class Promise<void>
    {
    public:
        void set_value();
    };
}