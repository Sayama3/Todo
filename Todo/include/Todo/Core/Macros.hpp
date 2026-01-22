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

// The thread count requirement macro definition/redefinition.
// Mainly used to be able to make the program work if no preprocessor definitions are given.

#ifndef TODO_MIN_THREAD_COUNT
	#define TODO_MIN_THREAD_COUNT 4
#else
	#if TODO_MIN_THREAD_COUNT <= 0
		#error "The number of thread needs to be at least one"
	#endif
#endif

#ifndef TODO_USE_ALL_THREADS
	#ifndef TODO_REMAINING_THREAD
		#define TODO_REMAINING_THREAD 2
	#endif
#else
	#ifdef TODO_REMAINING_THREAD
		#if TODO_REMAINING_THREAD != 0
			#error "The macro 'TODO_REMAINING_THREAD' is define and different from 0 even through the macro 'TODO_USE_ALL_THREADS' is already define."
		#endif
	#else
		#define TODO_REMAINING_THREAD 0
	#endif
#endif