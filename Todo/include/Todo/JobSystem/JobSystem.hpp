//
// Created by ianpo on 30/08/2024.
//

#pragma once

#include "Todo/Multithreading/Thread.hpp"

namespace Todo {

	class JobSystem {
	public:
		JobSystem(uint32_t threadCount);
		~JobSystem();
	private:
		void Poll();
	private:
		std::atomic<bool> m_Running{true};
		std::vector<Thread> m_Threads;
	};

} // Todo
