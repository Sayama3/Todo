//
// Created by ianpo on 12/01/2026.
//

#pragma once

#include "Future.hpp"
#include "Promise.hpp"

namespace Todo
{

    template <class>
    class PackagedTask; // not defined

    template<class ReturnType, class...ArgTypes>
    class PackagedTask<ReturnType(ArgTypes...)>
    {
    private:
        using FunctionType = std::function<ReturnType(ArgTypes...)>;
    public:
        PackagedTask();
        template< std::invocable<ReturnType, ArgTypes...> F >
        explicit PackagedTask( F&& f );
        explicit PackagedTask( const FunctionType& f );
        explicit PackagedTask( FunctionType&& f );
        PackagedTask( const PackagedTask& ) = delete;
        PackagedTask& operator=( const PackagedTask& ) = delete;
        PackagedTask( PackagedTask&& rhs ) noexcept;
        PackagedTask& operator=( PackagedTask&& rhs ) noexcept;
        void swap(PackagedTask& rhs) noexcept;
    public:
        [[nodiscard]] bool valid() const noexcept;
        [[nodiscard]] Future<ReturnType> get_future();
        void operator()(ArgTypes... args);
        void reset();
    private:
        FunctionType m_Function;
        Promise<ReturnType> m_Promise;
    };

    template <class ReturnType, class ... ArgTypes>
    PackagedTask<ReturnType(ArgTypes...)>::PackagedTask() = default;

    template <class ReturnType, class ... ArgTypes>
    template <std::invocable<ReturnType, ArgTypes...> F>
    PackagedTask<ReturnType(ArgTypes...)>::PackagedTask(F&& f) : m_Function(std::forward<F>(f))
    {
    }

    template <class ReturnType, class ... ArgTypes>
    PackagedTask<ReturnType(ArgTypes...)>::PackagedTask(const FunctionType& f) : m_Function(f)
    {
    }

    template <class ReturnType, class ... ArgTypes>
    PackagedTask<ReturnType(ArgTypes...)>::PackagedTask(FunctionType&& f) : m_Function(std::move(f))
    {
    }

    template <class ReturnType, class ... ArgTypes>
    PackagedTask<ReturnType(ArgTypes...)>::PackagedTask(PackagedTask&& rhs) noexcept
    {
        swap(this);
    }

    template <class ReturnType, class ... ArgTypes>
    PackagedTask<ReturnType(ArgTypes...)>& PackagedTask<ReturnType(ArgTypes...)>::operator=(PackagedTask&& rhs) noexcept
    {
        swap(this);
        return *this;
    }

    template <class ReturnType, class ... ArgTypes>
    void PackagedTask<ReturnType(ArgTypes...)>::swap(PackagedTask& rhs) noexcept
    {
        std::swap(m_Promise, rhs.m_Promise);
        std::swap(m_Function, rhs.m_Function);
    }

    template <class ReturnType, class ... ArgTypes>
    bool PackagedTask<ReturnType(ArgTypes...)>::valid() const noexcept
    {
        return m_Promise.is_valid();
    }

    template <class ReturnType, class ... ArgTypes>
    Future<ReturnType> PackagedTask<ReturnType(ArgTypes...)>::get_future()
    {
        return m_Promise.get_future();
    }

    template <class ReturnType, class ... ArgTypes>
    void PackagedTask<ReturnType(ArgTypes...)>::operator()(ArgTypes... args)
    {
        if (m_Promise.is_ready())
        {
            throw std::future_error(std::future_errc::promise_already_satisfied);
        }

        try
        {
            if constexpr (std::is_same_v<ReturnType, void>)
            {
                m_Function(std::forward<ArgTypes>(args)...);
                m_Promise.set_value();
            }
            else
            {
                ReturnType result = m_Function(std::forward<ArgTypes>(args)...);
                if constexpr (std::is_move_constructible_v<ReturnType>)
                {
                    m_Promise.set_value(std::move(result));
                } else
                {
                    m_Promise.set_value(result);
                }
            }
        }
        catch (...)
        {
            m_Promise.set_exception(std::current_exception());
        }
    }

    template <class ReturnType, class ... ArgTypes>
    void PackagedTask<ReturnType(ArgTypes...)>::reset()
    {
        *this = PackagedTask(m_Function);
    }
}
