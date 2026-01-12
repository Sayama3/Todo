//
// Created by Sayama on 28/08/2024.
//

#include "Todo/Core/Logger.hpp"
#include "Todo/Multithreading/Thread.hpp"

namespace Todo
{
	static inline uint64_t GetThreadIdAsInt(const Todo::Thread::ID id) {
		if constexpr (sizeof(Todo::Thread::ID) >= sizeof(uint64_t)) {
			return *(const uint64_t*)&id;
		}
		else if constexpr (sizeof(Todo::Thread::ID) >= sizeof(uint32_t)) {
			return *(const uint32_t*)&id;
		}
		else if constexpr (sizeof(Todo::Thread::ID) >= sizeof(uint16_t)) {
			return *(const uint16_t*)&id;
		}
		else {
			return *(const uint8_t*)&id;
		}
	}

	static inline std::string GetThreadIdAsStr(const Todo::Thread::ID id) {
		return std::to_string(GetThreadIdAsInt(id));
	}

	Thread::Thread() : m_Thread(), m_Name("Thread " + GetThreadIdAsStr(id())) {
	}

	Thread::Thread(InternalThread t) : m_Thread(std::move(t)), m_Name("Thread " + GetThreadIdAsStr(id())) {
	}

	Thread::Thread(std::string name) : m_Thread(), m_Name(std::move(name)) {
	}

	Thread::Thread(std::string name, InternalThread t) : m_Thread(std::move(t)), m_Name(std::move(name)) {
	}

	Thread::~Thread()
	{
		request_stop();
		try_join();
	}

	Thread::ID Thread::id() const {
		return m_Thread.get_id();
	}

	const std::string & Thread::name() const {
		return m_Name;
	}

	Thread::Thread(Thread &&lft)  noexcept : m_Thread(std::move(lft.m_Thread)), m_Name(std::move(lft.m_Name))
	{
	}

	Thread& Thread::operator=(Thread &&lft)  noexcept {
		swap(lft);
		return *this;
	}

	void Thread::swap(Thread& lft) noexcept {
		if(this == &lft) return;
		std::swap(/*this->*/m_Thread, lft.m_Thread);
		std::swap(/*this->*/m_Name, lft.m_Name);
	}

	void Thread::join() {
		TODO_ASSERT(joinable(), "The thread is trying to be Joined but cannot.");
		m_Thread.join();
	}

	void Thread::detach() {
		TODO_ASSERT(joinable(), "The thread is trying to be Detached but cannot.");
		m_Thread.detach();
	}

	bool Thread::joinable() const {
		return m_Thread.joinable();
	}

	bool Thread::try_join()
	{
		if(!joinable()) return false;
		join();
		return true;
	}

	bool Thread::try_detach()
	{
		if(!detachable()) return false;
		detach();
		return true;
	}

	void Thread::request_stop() {
		m_Thread.request_stop();
	}

	Thread::InternalThread &Thread::GetUnderlyingThread() {
		return m_Thread;
	}
}
