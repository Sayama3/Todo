//
// Created by ianpo on 01/09/2024.
//

#pragma once

#include "TMutex.hpp"

namespace Todo {

	template<typename T, typename Mut = std::mutex>
	class ThreadsafeQueue {
	public:
		ThreadsafeQueue() = default;
		~ThreadsafeQueue() = default;
		ThreadsafeQueue(const ThreadsafeQueue& other);
	public:
		ThreadsafeQueue& operator=(const ThreadsafeQueue& other) = delete;
		ThreadsafeQueue& operator=(ThreadsafeQueue&& other) = delete;
		ThreadsafeQueue(ThreadsafeQueue&& other) = delete;
	public:
		void push(T value);
		void wait_and_pop(T& value);
		std::shared_ptr<T> wait_and_pop();
		bool try_pop(T& value);
		std::shared_ptr<T> try_pop();
		bool empty() const;
		uint64_t count() const;
	private:
		std::queue<T> m_Queue;
		mutable TMutex<Mut> m_Mutex;
		std::condition_variable m_DataCond;
	};

	template<typename T, typename Mut>
	ThreadsafeQueue<T, Mut>::ThreadsafeQueue(const ThreadsafeQueue &other)
	{
		auto lock = other.m_Mutex.Guard();
		m_Queue = other.m_Queue;
	}

	template<typename T, typename Mut>
	void ThreadsafeQueue<T, Mut>::push(T value)
	{
		auto lock = m_Mutex.Guard();
		m_Queue.push(std::move(value));
		m_DataCond.notify_one();
	}
	template<typename T, typename Mut>
	void ThreadsafeQueue<T, Mut>::wait_and_pop(T &value)
	{
		auto unique = m_Mutex.UniqueGuard();
		m_DataCond.wait(unique, [this]{return !m_Queue.empty();});
		value = std::move(m_Queue.front());
		m_Queue.pop();
	}

	template<typename T, typename Mut>
	std::shared_ptr<T> ThreadsafeQueue<T, Mut>::wait_and_pop() {
		auto unique = m_Mutex.UniqueGuard();
		m_DataCond.wait(unique, [this]{return !m_Queue.empty();});
		std::shared_ptr<T> res (
			std::make_shared<T>(std::move(m_Queue.front()))
		);
		m_Queue.pop();
		return res;
	}

	template<typename T, typename Mut>
	bool ThreadsafeQueue<T, Mut>::try_pop(T &value) {
		auto guard = m_Mutex.Guard();
		if(m_Queue.empty()) return false;
		value = std::move(m_Queue.front());
		m_Queue.pop();
		return true;
	}

	template<typename T, typename Mut>
	std::shared_ptr<T> ThreadsafeQueue<T, Mut>::try_pop() {
		auto guard = m_Mutex.Guard();
		if(m_Queue.empty()) return nullptr;
		std::shared_ptr<T> res (
				std::make_shared<T>(std::move(m_Queue.front()))
		);
		m_Queue.pop();
		return res;
	}

	template<typename T, typename Mut>
	bool ThreadsafeQueue<T, Mut>::empty() const {
		auto guard = m_Mutex.Guard();
		return m_Queue.empty();
	}

	template<typename T, typename Mut>
	uint64_t ThreadsafeQueue<T, Mut>::count() const {
		auto guard = m_Mutex.Guard();
		return m_Queue.size();
	}

} // Todo
