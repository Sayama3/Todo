//
// Created by ianpo on 28/08/2024.
//

#include "Todo/Core/Logger.hpp"
#include "Todo/Multithreading/Thread.hpp"

namespace Todo
{

	Thread::Thread(std::thread t) : m_Thread(std::move(t)) {}
	Thread::Thread() = default;

	Thread::~Thread()
	{
		if (m_Thread.joinable()) {
			m_Thread.join();
		}
	}

	Thread::ID Thread::Id() const {
		return m_Thread.get_id();
	}

	Thread::Thread(Thread &&lft)  noexcept : m_Thread(std::move(lft.m_Thread))
	{
	}

	Thread& Thread::operator=(Thread &&lft)  noexcept {
		swap(lft);
		return *this;
	}

	void Thread::swap(Thread& lft)
	{
		if(this == &lft) return;
		std::swap(/*this->*/m_Thread, lft.m_Thread);
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
		m_Thread.join();
		return true;
	}

	bool Thread::TryDetach()
	{
		if(!Detachable()) return false;
		m_Thread.detach();
		return true;
	}

	std::thread &Thread::GetUnderlyingThread() { return m_Thread; }
}
