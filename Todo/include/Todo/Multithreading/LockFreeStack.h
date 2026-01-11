//
// Created by Sayama on 09/01/2026.
//

#pragma once
#include "Todo/Core/Allocator.hpp"

namespace Todo
{
    template <std::copyable T>
    class LockFreeStack
    {
        struct Node;

        struct CountedNodePtr
        {
            int external_count;
            Node* ptr;
        };

        struct Node
        {
            Node(const T& data) : data(std::make_shared<T>(data)), internal_count{0}
            {
            }

            std::shared_ptr<T> data;
            std::atomic<int> internal_count;
            CountedNodePtr next{nullptr};
        };

    public:
        ~LockFreeStack()
        {
            while (pop());
        }

    public:
        void push(const T& data)
        {
            CountedNodePtr node_ptr;
            node_ptr.ptr = Allocator::Create<Node>(data);
            node_ptr.external_count = 1;
            node_ptr.ptr->next = head.load(std::memory_order_relaxed);
            while (!head.compare_exchange_weak(node_ptr.ptr->next, node_ptr, std::memory_order_release,
                                               std::memory_order_relaxed));
        }

        std::shared_ptr<T> pop()
        {
            CountedNodePtr old_head = head.load(std::memory_order_relaxed);
            for (;;)
            {
                increase_head_count(old_head);
                Node* const ptr = old_head.ptr;
                if (!ptr)
                {
                    return nullptr;
                }

                if (head.compare_exchange_strong(old_head, ptr->next, std::memory_order_release,
                                                 std::memory_order_relaxed))
                {
                    std::shared_ptr<T> res;
                    res.swap(ptr->data);
                    const int count_increase = old_head.external_count - 2;
                    if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release) == -count_increase)
                    {
                        Allocator::Destroy(ptr);
                    }
                    return res;
                }
                else if (ptr->internal_count.fetch_add(1, std::memory_order_relaxed) == 1)
                {
                    ptr->internal_count.load(std::memory_order_acquire);
                    Allocator::Destroy(ptr);
                }
            }
        }

    private:
        void increase_head_count(CountedNodePtr& old_counter)
        {
            CountedNodePtr new_counter;
            do
            {
                new_counter = old_counter;
                ++new_counter.external_count;
            }
            while (!head.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire,
                                                 std::memory_order_relaxed));
            old_counter.external_count = new_counter.external_count;
        }

    private:
        std::atomic<CountedNodePtr> head;
    };
}
