//
// Created by ianpo on 12/01/2026.
//

#pragma once
#include "Future.hpp"

namespace Todo
{
    template<class Func, class...ArgTypes>
    class PackagedTask
    {
    public:
        PackagedTask() noexcept;
        template< class F >
        explicit PackagedTask( F&& f );
        PackagedTask( const PackagedTask& ) = delete;
        PackagedTask& operator=( const PackagedTask& ) = delete;
        PackagedTask( PackagedTask&& rhs ) noexcept;
        PackagedTask& operator=( PackagedTask&& rhs ) noexcept;
        void swap(PackagedTask& rhs) noexcept;
    public:
        bool valid() const noexcept;
        TODO::Future<Func> get_future();
        void operator()(ArgTypes... args);
        void reset();
    private:
        std::function<Func(ArgTypes...)> m_Function;
    };

    template <class Func, class ... ArgTypes>
    void PackagedTask<Func, ArgTypes...>::reset()
    {
        *this = packaged_task(std::move(m_Function));
    }
}
