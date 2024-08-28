#pragma once

#include <thread>

namespace todo
{
	class Thread
	{
	public:
		Thread();
		~Thread();
	private:
		std::thread m_Thread;
	};
}
