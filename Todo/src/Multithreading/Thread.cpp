//
// Created by ianpo on 28/08/2024.
//

#include "Todo/Core/Logger.hpp"
#include "Todo/Multithreading/Thread.hpp"

namespace Todo
{

	Thread::Thread(InternalThread t) : m_Thread(std::move(t)) {}
	Thread::Thread() : m_Thread() {}

	Thread::~Thread()
	{
		RequestStop();
		TryJoin();
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
