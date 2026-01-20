//
// Created by ianpo on 20/01/2026.
//

#pragma once

#include "Concepts.hpp"

namespace Todo
{

    class FunctionWrapper
    {
    private:
        struct impl_base
        {
            virtual void call() = 0;
            virtual ~impl_base() = default;
        };

        template<MovableVoidInvocable F>
        struct impl_type : public impl_base
        {
            inline impl_type(F&& f) : func(std::move(f)) {}
            inline void call() override {func();}
            F func;
        };
    public:
        FunctionWrapper() = default;
        FunctionWrapper(const FunctionWrapper&) = delete;
        FunctionWrapper(FunctionWrapper&) = delete;
        FunctionWrapper& operator=(const FunctionWrapper&) = delete;
        FunctionWrapper(FunctionWrapper&& o) noexcept : impl(std::move(o.impl)) {}
        FunctionWrapper& operator=(FunctionWrapper&& o) noexcept
        {
            swap(o);
            return *this;
        }
        void swap(FunctionWrapper& o) noexcept
        {
            std::swap(impl, o.impl);
        }

        template<MovableVoidInvocable F>
        FunctionWrapper(F&& f) : impl(new impl_type<F>(std::forward<F>(f))) {}

    public:
        void call()
        {
            impl->call();
        }

        void operator()()
        {
            call();
        }
    private:
        std::unique_ptr<impl_base> impl;
    };
}