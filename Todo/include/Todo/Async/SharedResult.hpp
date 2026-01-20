//
// Created by ianpo on 20/01/2026.
//

#pragma once

#include "Result.hpp"
#include "ResultVoid.hpp"

namespace Todo
{
    template <typename T>
    using SharedResult = std::shared_ptr<Result<T>>;

    using SharedVoidResult = std::shared_ptr<Result<void>>;

    template <typename T>
    static inline SharedResult<T> MakeSharedResult()
    {
        return std::make_shared<Result<T>>();
    }

    static inline SharedVoidResult MakeSharedVoidResult()
    {
        return std::make_shared<Result<void>>();
    }
}