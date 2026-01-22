//    Copyright 2026 https://github.com/Sayama3
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

        template<MovableInvocable F>
        struct impl_type : public impl_base
        {
            inline impl_type(F&& f) : func(std::move(f)) {}
            inline void call() override {func();}
            F func;
        };
    public:
        FunctionWrapper() = default;
        ~FunctionWrapper() = default;

        FunctionWrapper(FunctionWrapper&) = delete;
        FunctionWrapper(const FunctionWrapper&) = delete;
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

        template<typename F>
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

    static_assert(std::is_move_assignable_v<FunctionWrapper>);
    static_assert(std::is_move_constructible_v<FunctionWrapper>);
    static_assert(std::is_swappable_v<FunctionWrapper>);

}