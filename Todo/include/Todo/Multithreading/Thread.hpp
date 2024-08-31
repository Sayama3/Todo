//
// Created by ianpo on 28/08/2024.
//

#pragma once

namespace Todo
{
	class Thread
	{
	public:
		using ID = std::thread::id;
	public:
		Thread();
		explicit Thread(std::thread t);
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
	public:
		[[nodiscard]] ID Id() const;
		[[nodiscard]] bool Joinable() const;
		[[nodiscard]] bool Detachable() const {return Joinable();}

		void Join();
		void Detach();

		bool TryJoin();
		bool TryDetach();
	public:
		std::thread& GetUnderlyingThread();
		void swap(Thread& lft);
	private:
		std::thread m_Thread;
	};

	// ===== Implementation =====
	template<typename Func>
	Thread::Thread(Func func) : m_Thread(func) {}

	template<typename Func, typename... Args>
	Thread::Thread(Func func, Args &&... args) : m_Thread(func, std::forward<Args>(args)...) {}
}
