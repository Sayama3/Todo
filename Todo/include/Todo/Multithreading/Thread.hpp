//
// Created by ianpo on 28/08/2024.
//

#pragma once

namespace Todo
{
	class Thread
	{
	public:
		//TODO: Move as template ?
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
		[[nodiscard]] ID Id() const;
		[[nodiscard]] const std::string& Name() const;

		[[nodiscard]] bool Joinable() const;
		[[nodiscard]] bool Detachable() const {return Joinable();}

		void Join();
		void Detach();

		bool TryJoin();
		bool TryDetach();

		void RequestStop();
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
