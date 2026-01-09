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
        T* Create(Args&& ... args)
        {
            void* data = alloc(sizeof(T));
            TODO_ASSERT(data != nullptr, "cannot allocate data");
            std::construct_at<T>(data, std::forward<Args>(args)...);
            return static_cast<T*>(data);
        }

        template<typename T>
        void Delete(T* ptr)
        {
            if (!ptr)
            {
                return;
            }

            std::destroy_at(ptr);
            dealloc(ptr);
        }


        void* (*alloc)(std::size_t size) = &malloc;
        void (*dealloc)(void* ptr) = &free;
    };

    inline Allocator s_Allocator{};


}