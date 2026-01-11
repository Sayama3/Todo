//
// Created by Sayama on 09/01/2026.
//

#pragma once

#include "Logger.hpp"

namespace Todo
{
    struct Allocator
    {
        template<typename T, typename ... Args>
        T* create(Args&& ... args)
        {
            void* data = alloc(sizeof(T));
            TODO_ASSERT(data != nullptr, "cannot allocate data");
            std::construct_at<T>(data, std::forward<Args>(args)...);
            return static_cast<T*>(data);
        }
        template<typename T, typename ... Args>
        static T* Create(Args&& ... args);

        template<typename T>
        static void Destroy(T* ptr);

        template<typename T>
        void destroy(T* ptr)
        {
            if (!ptr)
            {
                return;
            }

            std::destroy_at(ptr);
            dealloc(ptr);
        }


        void* (*alloc)(std::size_t size) = &::operator new; // Could be malloc
        void (*dealloc)(void* ptr) = &::operator delete; // Could be free
    };

    inline Allocator s_Allocator{};

    template <typename T, typename ... Args>
    T* Allocator::Create(Args&&... args)
    {
        return s_Allocator.create<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    void Allocator::Destroy(T* ptr)
    {
        s_Allocator.destroy(ptr);
    }

    template<typename T>
    struct TAllocator
    {
        typedef T value_type;

        T* allocate(const size_t n) const
        {
            return static_cast<T*>(s_Allocator.alloc(n * sizeof(T)));
        }

        void deallocate(T *const p, const size_t n) const noexcept
        {
            s_Allocator.dealloc(p);
        }

        template<class U>
        constexpr bool operator==(const TAllocator<U> &) const noexcept {
            return true;
        }

        template<class U>
        constexpr bool operator!=(const TAllocator<U> &) const noexcept {
            return false;
        }
    };

    template<typename T>
    struct TDeleter
    {
        typedef T value_type;

        constexpr void operator()(T* p) const noexcept {
            TAllocator<T> allocator{};
            std::destroy_at(p);
            allocator.deallocate(p);
        }
    };

}