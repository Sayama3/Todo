//
// Created by ianpo on 12/01/2026.
//

#pragma once

namespace Todo
{
    template<typename T>
    class SharedResult
    {

    private:
        T* result{nullptr};
        std::atomic_uint64_t count{};
    };

    template<>
    class SharedResult<void>
    {

    };
}