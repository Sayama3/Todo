//
// Created by ianpo on 01/09/2024.
//

#pragma once

#include "TMutex.hpp"

namespace Todo {

	template<typename T, typename Mut = std::mutex>
	class SafeQueue
	{
	private:
		using Mutex = TMutex<Mut>;
		struct Node {
			Node() : data{nullptr}, next{nullptr} {}
			Node(std::shared_ptr<T> data) : data(std::move(data)) {}
			~Node() = default;
			std::shared_ptr<T> data;
			std::unique_ptr<Node> next;
		};
	public:
		SafeQueue() : m_Head(new Node), m_Tail(m_Head.get()) {}
		SafeQueue(const SafeQueue&) = delete;
		SafeQueue& operator=(const SafeQueue&) = delete;
		~SafeQueue();
	public:
		std::shared_ptr<T> try_pop();
		bool try_pop(T& value);

		std::shared_ptr<T> wait_and_pop();
		void wait_and_pop(T& value);

		void push(T new_value);

		void clear();

		uint64_t count() const;
		uint64_t calculate_count() const;
		bool empty() const;
	private:
		bool unguard_empty() const;
		std::unique_ptr<Node> pop_head();
		Node* get_tail();
		UniqueLockguard<Mutex> wait_for_data();
		std::unique_ptr<Node> wait_pop_head();
		std::unique_ptr<Node> wait_pop_head(T& value);
		std::unique_ptr<Node> try_pop_head();
		std::unique_ptr<Node> try_pop_head(T& value);
		void unguard_clear();

	private:
		std::unique_ptr<Node> m_Head;
		Node* m_Tail;
		mutable Mutex m_HeadMutex;
		mutable Mutex m_TailMutex;
		std::atomic<uint64_t> m_Count {0ull};
		std::condition_variable m_DataCond;
	};

	template<typename T, typename Mut>
	std::unique_ptr<typename SafeQueue<T, Mut>::Node> SafeQueue<T, Mut>::try_pop_head() {
		auto head_lock = m_HeadMutex.Guard();
		if (m_Head.get() == get_tail()) {
			return std::unique_ptr<Node>();
		}
		return pop_head();
	}

	template<typename T, typename Mut>
	std::unique_ptr<typename SafeQueue<T, Mut>::Node> SafeQueue<T, Mut>::try_pop_head(T &value) {
		auto head_lock = m_HeadMutex.Guard();
		if (m_Head.get() == get_tail()) {
			return std::unique_ptr<Node>();
		}
		value = std::move(*m_Head->data);
		return pop_head();
	}

	template<typename T, typename Mut>
	void SafeQueue<T, Mut>::clear()
	{
		auto head_lock = m_HeadMutex.Guard();
		auto tail_lock = m_TailMutex.Guard();

		unguard_clear();
	}
	template<typename T, typename Mut>
	void SafeQueue<T, Mut>::unguard_clear()
	{
		while (!unguard_empty())
		{
			pop_head();
		}
	}


	template<typename T, typename Mut>
	std::unique_ptr<typename SafeQueue<T,Mut>::Node> SafeQueue<T, Mut>::pop_head() {
		std::unique_ptr<Node> old_head = std::move(m_Head);
		m_Head = std::move(old_head->next);
		m_Count.fetch_sub(1, std::memory_order_relaxed);
		return old_head;
	}

	template<typename T, typename Mut>
	SafeQueue<T,Mut>::Node *SafeQueue<T, Mut>::get_tail() {
		auto lock_guard = m_TailMutex.Guard();
		return m_Tail;
	}

	template<typename T, typename Mut>
	UniqueLockguard<typename Todo::SafeQueue<T, Mut>::Mutex> SafeQueue<T, Mut>::wait_for_data() {
		UniqueLockguard<Todo::SafeQueue<T, Mut>::Mutex> head_lock = m_HeadMutex.UniqueGuard();
		m_DataCond.wait(head_lock, [&]{return m_Head.get() != get_tail();});
		return std::move(head_lock);
	}

	template<typename T, typename Mut>
	std::unique_ptr<typename SafeQueue<T, Mut>::Node> SafeQueue<T, Mut>::wait_pop_head() {
		auto head_lock{wait_for_data()};
		return pop_head();
	}

	template<typename T, typename Mut>
	std::unique_ptr<typename SafeQueue<T, Mut>::Node> SafeQueue<T, Mut>::wait_pop_head(T &value) {
		auto head_lock{wait_for_data()};
		value = std::move(*m_Head->data);
		return pop_head();
	}

	template<typename T, typename Mut>
	void SafeQueue<T, Mut>::wait_and_pop(T &value) {
		auto const old_head = wait_pop_head(value);
	}

	template<typename T, typename Mut>
	std::shared_ptr<T> SafeQueue<T, Mut>::wait_and_pop() {
		auto const old_head = wait_pop_head();
		return old_head->data;
	}

	template<typename T, typename Mut>
	SafeQueue<T, Mut>::~SafeQueue() {
		unguard_clear();
	}

	template<typename T, typename Mut>
	std::shared_ptr<T> SafeQueue<T, Mut>::try_pop() {
		std::unique_ptr<Node> old_head = try_pop_head();
		return old_head ? old_head->data : std::shared_ptr<T>();
	}

	template<typename T, typename Mut>
	bool SafeQueue<T, Mut>::try_pop(T &value) {
		std::unique_ptr<Node> old_head = try_pop_head(value);
		return old_head;
	}

	template<typename T, typename Mut>
	void SafeQueue<T, Mut>::push(T new_value)
	{
		// Constructing the shared pointer for the data.
		auto new_data {std::make_shared<T>(std::move(new_value))};

		// Creating the new dummy node
		auto new_dummy_node {std::make_unique<Node>()};

		// Fetching the pointer to the new dummy node.
		Node* const new_tail = new_dummy_node.get();

		// Only locking the operations on the tail.
		{
			auto lock = m_TailMutex.Guard();

			// Setting the data on the previous dummy node. Becoming the last data node.
			m_Tail->data = new_data;

			// Setting the dummy node a the following on the linked list.
			m_Tail->next = std::move(new_dummy_node);

			// Updating the tail pointer.
			m_Tail = new_tail;

			m_Count.fetch_add(1, std::memory_order_relaxed);
		}

		m_DataCond.notify_one();
	}

	template<typename T, typename Mut>
	bool SafeQueue<T, Mut>::empty() const {
		auto head_guard = m_HeadMutex.Guard();
		return m_Head.get() == get_tail();
	}

	template<typename T, typename Mut>
	bool SafeQueue<T, Mut>::unguard_empty() const {
		return m_Head.get() == m_Tail;
	}

	template<typename T, typename Mut>
	uint64_t SafeQueue<T, Mut>::calculate_count() const {
		auto head_guard = m_HeadMutex.Guard();
		auto tail_guard = m_TailMutex.Guard();

		uint64_t count{0};
		Node* node = m_Head.get();
		while (node != m_Tail) {
			++count;
			node = node->next.get();
		}
		return count;
	}
	template<typename T, typename Mut>
	uint64_t SafeQueue<T, Mut>::count() const {
		return m_Count.load(std::memory_order_acquire);
	}

} // Todo
