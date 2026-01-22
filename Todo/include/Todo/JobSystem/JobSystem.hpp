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

#include "Todo/Multithreading/Thread.hpp"

namespace Todo {

	class JobSystem {
	public:
		JobSystem(uint32_t threadCount);
		~JobSystem();
	private:
		void Poll();
	private:
		std::atomic<bool> m_Running{false};
		std::vector<Thread> m_Threads;
	};

} // Todo
