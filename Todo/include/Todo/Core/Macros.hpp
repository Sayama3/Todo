//
// Created by ianpo on 30/08/2024.
//

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