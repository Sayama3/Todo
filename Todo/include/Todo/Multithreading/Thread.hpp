//    Copyright 2026 https://github.com/Sayama3
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
