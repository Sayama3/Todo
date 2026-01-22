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
