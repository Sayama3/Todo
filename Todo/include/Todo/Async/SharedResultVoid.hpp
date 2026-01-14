//
// Created by ianpo on 14/01/2026.
//

#pragma once

#include "SharedResult.hpp"

// TODO: Make the void specialization if I ever need one.
// namespace Todo
// {
//     template <typename Alloc>
//     class SharedResult<void, Alloc>
//     {
//         using TypePtr = uint64_t;
//         using ErrPtr = std::exception_ptr*;
//
//         enum ValueType : uint8_t
//         {
//             V_None = 0,
//             V_Value,
//             V_Error,
//         };
//
//         struct Result
//         {
//             ValueType type{};
//
//             union
//             {
//                 TypePtr value{0ull};
//                 ErrPtr error;
//             };
//         };
//     public:
//         SharedResult();
//         ~SharedResult();
//         SharedResult(const SharedResult& o);
//         SharedResult& operator=(const SharedResult& o);
//         SharedResult(SharedResult&& o) noexcept;
//         SharedResult& operator=(SharedResult&& o) noexcept;
//         void swap(SharedResult& o) noexcept;
//
//     public:
//         [[maybe_unused]] void set_value();
//
//         [[maybe_unused]] void set_exception(const std::exception_ptr& exception);
//
//         [[nodiscard]] bool get();
//
//         [[nodiscard]] T wait_and_get();
//
//         [[nodiscard]] bool is_ready() const;
//
//         [[nodiscard]] bool has_value() const;
//
//         [[nodiscard]] bool has_error() const;
//     private:
//         void release();
//     private:
//         std::atomic_uint64_t* p_Count{nullptr};
//         std::atomic<Result>* p_Result{nullptr};
//         std::atomic_flag* p_Ready{nullptr};
//     };
//
//
//     template<typename Alloc>
//     SharedResult<void, Alloc>::SharedResult()
//     {
//     }
//
//     template<typename Alloc>
//     SharedResult<void, Alloc>::~SharedResult()
//     {
//     }
//
//     template<typename Alloc>
//     SharedResult<void, Alloc>::SharedResult(const SharedResult<void>& o)
//     {
//     }
//
//     template<typename Alloc>
//     SharedResult<void>& SharedResult<void, Alloc>::operator=(const SharedResult<void>& o)
//     {
//     }
//
//     template<typename Alloc>
//     SharedResult<void, Alloc>::SharedResult(SharedResult<void>&& o) noexcept
//     {
//     }
//
//     template<typename Alloc>
//     SharedResult<void>& SharedResult<void, Alloc>::operator=(SharedResult<void>&& o) noexcept
//     {
//     }
//
//     template<typename Alloc>
//     void SharedResult<void, Alloc>::swap(SharedResult<void>& o) noexcept
//     {
//     }
//
//     template<typename Alloc>
//     void SharedResult<void, Alloc>::set_value()
//     {
//     }
//
//     template<typename Alloc>
//     void SharedResult<void, Alloc>::set_exception(const std::exception_ptr& exception)
//     {
//     }
//
//     template<typename Alloc>
//     bool SharedResult<void, Alloc>::get()
//     {
//     }
//
//     template<typename Alloc>
//     bool SharedResult<void, Alloc>::is_ready() const
//     {
//         return p_
//     }
//
//     template<typename Alloc>
//     bool SharedResult<void, Alloc>::has_value() const
//     {
//     }
//
//     template<typename Alloc>
//     bool SharedResult<void, Alloc>::has_error() const
//     {
//     }
//
//     template <typename Alloc>
//     void SharedResult<void, Alloc>::release()
//     {
//         if (!p_Count)
//         {
//             return;
//         }
//         TODO_ASSERT(p_Result, "No valid pointer for p_Result");
//
//         const auto c = p_Count->fetch_sub(1, std::memory_order_acquire);
//         if (c == 1)
//         {
//             Result res = p_Result->exchange(Result{});
//             if (res.value)
//             {
//                 switch (res.type)
//                 {
//                 case V_Error:
//                     {
//                         delete res.error;
//                         break;
//                     }
//                 default:
//                     {
//                         break;
//                     }
//                 }
//             }
//
//             delete p_Result;
//             delete p_Count;
//             delete p_Ready;
//         }
//
//         p_Result = nullptr;
//         p_Count  = nullptr;
//         p_Ready  = nullptr;
//     }
// }
