//
// Created by ianpo on 11/01/2026.
//

#pragma once

#include "Todo/Core/Allocator.hpp"

namespace Todo {

	template<std::copyable T>
	class LockFreeQueue
	{
	private:
		struct Node {
			Node() : data{nullptr}, next{nullptr} {}
			Node(std::shared_ptr<T> data) : data(std::move(data)), next(nullptr)
			{
			}

			~Node() = default;
			std::shared_ptr<T> data;
			Node* next;
		};
	public:
		LockFreeQueue() : m_Head(Allocator::Create<Node>()), m_Tail(m_Head.load()) {}
		LockFreeQueue(const LockFreeQueue&) = delete;
		LockFreeQueue& operator=(const LockFreeQueue&) = delete;
		~LockFreeQueue();
	public:
		std::shared_ptr<T> pop();
		void push(T new_value);

		void clear();
	private:
		Node* pop_head();
	private:
		std::atomic<Node*> m_Head;
		std::atomic<Node*> m_Tail;
		std::atomic<uint64_t> m_Count {0ull};
	};

	template <std::copyable T>
	LockFreeQueue<T>::~LockFreeQueue()
	{
		clear();
	}

	template <std::copyable T>
	std::shared_ptr<T> LockFreeQueue<T>::pop()
	{
		Node* old_head = pop_head();
		if(!old_head)
		{
			return nullptr;
		}
		const auto res ={old_head->data};
		Allocator::Destroy(old_head);
		return res;
	}

	template <std::copyable T>
	void LockFreeQueue<T>::push(T new_value)
	{
		auto new_data = std::make_shared<T>(new_value);
		Node* p = Allocator::Create<Node>();
		Node* const old_tail = m_Tail.load(std::memory_order_seq_cst);
		old_tail->data.swap(new_data);
		old_tail->next = p;
		m_Tail.store(p, std::memory_order_seq_cst);
	}

	template <std::copyable T>
	void LockFreeQueue<T>::clear()
	{
		while (Node* const old_head = m_Head.load(std::memory_order_seq_cst))
		{
			m_Head.store(old_head->next);
			Allocator::Destroy<Node>(old_head);
		}
	}

	template <std::copyable T>
	typename LockFreeQueue<T>::Node* LockFreeQueue<T>::pop_head()
	{
		Node* const old_head = m_Head.load(std::memory_order_seq_cst);
		if (old_head == m_Tail.load(std::memory_order_seq_cst))
		{
			return nullptr;
		}
		m_Head.store(old_head->next, std::memory_order_seq_cst);
		return old_head;
	}
} // Todo
