//
// Created by ianpo on 20/01/2026.
//

#pragma once

#include "Result.hpp"
#include "ResultVoid.hpp"

namespace Todo
{
    template <typename T, typename Alloc = TAllocator<T>>
    using SharedResult = std::shared_ptr<Result<T, Alloc>>;

    using SharedVoidResult = std::shared_ptr<Result<void>>;

    template <typename T, typename Alloc = TAllocator<T>>
    static inline SharedResult<T, Alloc> MakeSharedResult()
    {
        return std::make_shared<Result<T, Alloc>>();
    }

    static inline SharedVoidResult MakeSharedVoidResult()
    {
        return std::make_shared<Result<void>>();
    }
}