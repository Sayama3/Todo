//
// Created by Sayama on 28/08/2024.
//

#pragma once

namespace Todo
{
	class Thread
	{
	public:
		using InternalThread = std::jthread;
		using ID = std::jthread::id;
	public:
		Thread();
		Thread(InternalThread t);
		Thread(std::string name);
		Thread(std::string name, InternalThread t);
		~Thread();
		Thread(Thread&& lft) noexcept;
		Thread& operator=(Thread&& lft) noexcept;

		Thread(const Thread&) = delete;
		Thread& operator=(const Thread&) = delete;
	public:
		template<typename Func>
		[[maybe_unused]] Thread(Func func);

		template<typename Func, typename ... Args>
		[[maybe_unused]] Thread(Func func, Args&& ... args);

		template<typename Func>
		[[maybe_unused]] Thread(std::string name, Func func);

		template<typename Func, typename ... Args>
		[[maybe_unused]] Thread(std::string name, Func func, Args&& ... args);
	public:
		[[nodiscard]] ID id() const;
		[[nodiscard]] const std::string& name() const;

		[[nodiscard]] bool joinable() const;
		[[nodiscard]] bool detachable() const {return joinable();}

		void join();
		void detach();

		bool try_join();
		bool try_detach();

		void request_stop();
	public:
		InternalThread& GetUnderlyingThread();
		void swap(Thread& lft) noexcept;
	private:
		InternalThread m_Thread;
		std::string m_Name;
	};

	// ===== Implementation =====
	template<typename Func>
	Thread::Thread(Func func) : Thread(Thread::InternalThread(func)) {}

	template<typename Func, typename... Args>
	Thread::Thread(Func func, Args&&... args) : Thread(Thread::InternalThread(func, std::forward<Args>(args)...)) {}

	template<typename Func>
	Thread::Thread(std::string name, Func func) : Thread(std::move(name), Thread::InternalThread(func)) {}

	template<typename Func, typename... Args>
	Thread::Thread(std::string name, Func func, Args&&... args) : Thread(std::move(name), Thread::InternalThread(func, std::forward<Args>(args)...)) {}
}
