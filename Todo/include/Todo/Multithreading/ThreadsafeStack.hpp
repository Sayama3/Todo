//
// Created by ianpo on 01/09/2024.
//

#pragma once

#include "Mutex.hpp"

namespace Todo {

	struct empty_stack : std::logic_error
	{
		empty_stack();
	};

	template<typename T>
	class ThreadsafeStack
	{
	public:
		ThreadsafeStack() = default;
		~ThreadsafeStack() = default;
		ThreadsafeStack(const ThreadsafeStack& other);
		ThreadsafeStack& operator=(const ThreadsafeStack&) = delete;
	public:

		/**
		 * Add the value to the top of the stack.
		 * @param value
		 */
		void push(T value);

		/**
		 * Return a shared_ptr to the previous top value.
		 * Might failed and throw an empty_stack error.
		 * @return Whether the value has been set.
		 */
		void pop(T& value);

		/**
		 * Return a shared_ptr to the previous top value.
		 * Might failed and throw an empty_stack error.
		 * @return The top value of the stack.
		 */
		std::shared_ptr<T> pop();

		/**
		 * Return a shared_ptr to the previous top value.
		 * Might failed and return false (if stack is empty).
		 * @return Whether the value has been set.
		 */
		bool try_pop(T& value);

		/**
		 * Return a shared_ptr to the previous top value.
		 * Might failed and return nullptr (if stack is empty).
		 * @return The top value of the stack.
		 */
		std::shared_ptr<T> try_pop();

		bool empty() const;

		/**
		 * Fetch the number of element in the stack at the moment of the lock.
		 * Ensure on your own no other threads use the stack after
		 * the count is done if you want the value to stay up to date.
		 * @return The number of element in the stack.
		 */
		uint64_t count() const;
	private:
		std::stack<T> m_Stack;
		mutable Mutex m_Mutex;
	};

	template<typename T>
	uint64_t ThreadsafeStack<T>::count() const {
		auto guard = m_Mutex.Guard();
		return m_Stack.size();
	}

	template<typename T>
	ThreadsafeStack<T>::ThreadsafeStack(const ThreadsafeStack &other) {
		auto guard = other.m_Mutex.Guard();
		m_Stack = other.m_Stack;
	}

	template<typename T>
	bool ThreadsafeStack<T>::empty() const
	{
		auto lock = m_Mutex.Guard();
		return m_Stack.empty();
	}

	template<typename T>
	void ThreadsafeStack<T>::push(T value)
	{
		auto lock = m_Mutex.Guard();
		m_Stack.push(std::move(value));
	}

	template<typename T>
	void ThreadsafeStack<T>::pop(T &value)
	{
		auto lock = m_Mutex.Guard();
		if (m_Stack.empty()) throw empty_stack();
		value = std::move(m_Stack.top());
		m_Stack.pop();
	}

	template<typename T>
	std::shared_ptr<T> ThreadsafeStack<T>::pop() {

		auto lock = m_Mutex.Guard();
		if (m_Stack.empty()) throw empty_stack();
		std::shared_ptr<T> const res(
				std::make_shared<T>(std::move(m_Stack.top()))
		);
		m_Stack.pop();
	}

	template<typename T>
	bool ThreadsafeStack<T>::try_pop(T &value)
	{
		auto lock = m_Mutex.Guard();
		if (m_Stack.empty()) return false;
		value = std::move(m_Stack.top());
		m_Stack.pop();
		return true;
	}

	template<typename T>
	std::shared_ptr<T> ThreadsafeStack<T>::try_pop() {

		auto lock = m_Mutex.Guard();
		if (m_Stack.empty()) return nullptr;
		std::shared_ptr<T> const res(
				std::make_shared<T>(std::move(m_Stack.top()))
		);
		m_Stack.pop();
		return res;
	}

} // Todo
