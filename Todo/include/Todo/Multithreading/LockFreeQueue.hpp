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

#include "Todo/Core/Allocator.hpp"

namespace Todo
{
	template<std::move_constructible T, typename TAlloc = TAllocator<T>, typename TDeleter = TDeleter<T>>
	class LockFreeQueue
	{
	public:
		using UniquePtr = std::unique_ptr<T, TDeleter>;
		using SharedPtr = std::shared_ptr<T>;

		template<typename ... Args>
		inline static T* Construct(Args&&... args)
		{
			TAlloc alloc;
			T* ptr = alloc.allocate(1);
			std::construct_at(ptr);
			return ptr;
		}

		template<typename... Args>
		inline static SharedPtr MakeShared(Args&&...args)
		{
			return std::allocate_shared<T, TAlloc>(TAlloc{}, std::forward<Args>(args)...);
		}

		template<typename... Args>
		inline static UniquePtr MakeUnique(Args&&...args)
		{
			return UniquePtr(TDeleter{}, Construct(std::forward<Args>(args)...));
		}
	private:
		struct Node;
		struct CountedNodePtr
		{
			int external_count;
			Node* ptr;
		};
		struct NodeCounter
		{
			uint32_t internal_count:30;
			uint32_t external_count:2;
		};
		struct Node {
			Node();
			// Node(std::shared_ptr<T> data);
			~Node() = default;

			void release_ref();

			std::atomic<T*> data;
			std::atomic<CountedNodePtr> next;
			std::atomic<NodeCounter> counter;
		};
	public:
		LockFreeQueue() : m_Head(Allocator::Create<Node>()), m_Tail(m_Head.load()) {}
		LockFreeQueue(const LockFreeQueue&) = delete;
		LockFreeQueue& operator=(const LockFreeQueue&) = delete;
		~LockFreeQueue();
	public:
		UniquePtr pop();
		void push(T new_value);

		void clear();
	private:
		static void increase_external_count(std::atomic<CountedNodePtr>& counter, CountedNodePtr& old);
		static void free_external_counter(CountedNodePtr& old);
	private:
		void set_new_tail(CountedNodePtr& old_tail, const CountedNodePtr& new_tail);
	private:
		std::atomic<CountedNodePtr> m_Head;
		std::atomic<CountedNodePtr> m_Tail;
		std::atomic<uint64_t> m_Count {0ull};
		TAlloc allocator;
	};

	template <std::move_constructible T, typename Alloc, typename TDeleter>
	LockFreeQueue<T, Alloc, TDeleter>::Node::Node()
	{
		NodeCounter new_counter;
		new_counter.internal_count = 0;
		new_counter.external_count = 2;
		counter.load(new_counter, std::memory_order_seq_cst);

		next.external_count = 0;
		next.ptr = nullptr;
	}

	template <std::move_constructible T, typename TAlloc, typename TDeleter>
	void LockFreeQueue<T, TAlloc, TDeleter>::Node::release_ref()
	{
		NodeCounter old_counter = counter.load(std::memory_order_relaxed);
		NodeCounter new_counter;
		do
		{
			new_counter = old_counter;
			--new_counter.internal_count;
		} while (!counter.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));

		if (!new_counter.internal_count && !new_counter.external_count)
		{
			// This function is used in place of the destructor and the deallocator.
			Allocator::Destroy(this);
		}
	}

	template <std::move_constructible T, typename Alloc, typename TDeleter>
	LockFreeQueue<T, Alloc, TDeleter>::~LockFreeQueue()
	{
		clear();
	}

	template <std::move_constructible T, typename Alloc, typename TDeleter>
	LockFreeQueue<T, Alloc, TDeleter>::UniquePtr LockFreeQueue<T, Alloc, TDeleter>::pop()
	{
		CountedNodePtr old_head = m_Head.load(std::memory_order_relaxed);
		while (true)
		{
			increase_external_count(m_Head, old_head);
			Node*const ptr = old_head.ptr;
			if (ptr == m_Tail.load(std::memory_order_seq_cst).ptr)
			{
				ptr->release_ref();
				return nullptr;
			}
			CountedNodePtr next = ptr->next.load(std::memory_order_seq_cst);
			if (m_Head.compare_exchange_strong(old_head, next))
			{
				T* const res = ptr->data.exchange(nullptr, std::memory_order_seq_cst);
				free_external_counter(old_head);
				return UniquePtr{TDeleter{}, res};
			}
			ptr->release_ref();
		}
	}

	template <std::move_constructible T, typename Alloc, typename TDeleter>
	void LockFreeQueue<T, Alloc, TDeleter>::push(T new_value)
	{
		UniquePtr new_data = MakeUnique(std::move(new_value));

		CountedNodePtr new_next;
		new_next.ptr = Allocator::Create<Node>();
		new_next.external_count = 1;

		CountedNodePtr old_tail = m_Tail.load(std::memory_order_seq_cst);
		while (true)
		{
			increase_external_count(m_Tail, old_tail);
			T* old_data = nullptr;
			if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get(), std::memory_order_seq_cst, std::memory_order_relaxed))
			{
				CountedNodePtr old_next = {0};
				if (!old_tail.ptr->next.compare_exchange_strong(old_next, new_next))
				{
					Allocator::Destroy(new_next.ptr);
					new_next = old_next;
				}

				set_new_tail(old_tail, new_next);
				new_data.reset(); // release ???
				break;
			}
			else
			{
				CountedNodePtr old_next{0};
				if (old_tail.ptr->ptr.compare_exchange_strong(old_next, new_next, std::memory_order_seq_cst, std::memory_order_relaxed))
				{
					old_next = new_next;
					new_next.ptr = Allocator::Create<Node>();
				}
				set_new_tail(old_tail, old_next);
			}
		}
	}

	template <std::move_constructible T, typename Alloc, typename TDeleter>
	void LockFreeQueue<T, Alloc, TDeleter>::clear()
	{
		while (pop());
	}

	template <std::move_constructible T, typename TAlloc, typename TDeleter>
	void LockFreeQueue<T, TAlloc, TDeleter>::increase_external_count(std::atomic<CountedNodePtr>& counter, CountedNodePtr& old)
	{
		CountedNodePtr new_counter;
		do
		{
			new_counter = old;
			++new_counter.external_count;
		} while (counter.compare_exchange_strong(old, new_counter, std::memory_order_acquire, std::memory_order_relaxed));
		old.external_count = new_counter.external_count;
	}

	template <std::move_constructible T, typename TAlloc, typename TDeleter>
	void LockFreeQueue<T, TAlloc, TDeleter>::free_external_counter(CountedNodePtr& old)
	{
		Node* const ptr = old.ptr;
		int const count_increase = old.external_count - 2;
		NodeCounter old_counter = ptr->count.load(std::memory_order_relaxed);
		NodeCounter new_counter;
		do
		{
			new_counter = old_counter;
			--new_counter.external_count;
			new_counter.internal_count += count_increase;
		} while (!ptr->counter.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));

		if (!new_counter.internal_count && !new_counter.external_count)
		{
			Allocator::Destroy(ptr);
		}
	}

	template <std::move_constructible T, typename TAlloc, typename TDeleter>
	void LockFreeQueue<T, TAlloc, TDeleter>::set_new_tail(CountedNodePtr& old_tail, const CountedNodePtr& new_tail)
	{
		Node* const current_tail_ptr = old_tail.ptr;
		while (!m_Tail.compare_exchange_weak(old_tail, new_tail, std::memory_order_seq_cst, std::memory_order_relaxed) && old_tail.ptr == current_tail_ptr);

		if (old_tail.ptr ==  current_tail_ptr)
		{
			free_external_counter(old_tail);
		}
		else
		{
			current_tail_ptr->release_ref();
		}
	}
} // Todo
