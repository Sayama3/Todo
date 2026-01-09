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

	Thread::Thread() : m_Thread(), m_Name("Thread " + GetThreadIdAsStr(Id())) {
	}

	Thread::Thread(InternalThread t) : m_Thread(std::move(t)), m_Name("Thread " + GetThreadIdAsStr(Id())) {
	}

	Thread::Thread(std::string name) : m_Thread(), m_Name(std::move(name)) {
	}

	Thread::Thread(std::string name, InternalThread t) : m_Thread(std::move(t)), m_Name(std::move(name)) {
	}

	Thread::~Thread()
	{
		RequestStop();
		TryJoin();
	}

	Thread::ID Thread::Id() const {
		return m_Thread.get_id();
	}

	const std::string & Thread::Name() const {
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

	void Thread::Join() {
		TODO_ASSERT(Joinable(), "The thread is trying to be Joined but cannot.");
		m_Thread.join();
	}

	void Thread::Detach() {
		TODO_ASSERT(Joinable(), "The thread is trying to be Detached but cannot.");
		m_Thread.detach();
	}

	bool Thread::Joinable() const {
		return m_Thread.joinable();
	}

	bool Thread::TryJoin()
	{
		if(!Joinable()) return false;
		Join();
		return true;
	}

	bool Thread::TryDetach()
	{
		if(!Detachable()) return false;
		Detach();
		return true;
	}

	void Thread::RequestStop() {
		m_Thread.request_stop();
	}

	Thread::InternalThread &Thread::GetUnderlyingThread() {
		return m_Thread;
	}
}
