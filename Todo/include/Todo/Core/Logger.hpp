//
// Created by ianpo on 30/08/2024.
//

#pragma once

//TODO: Handle a way to pass message

namespace Todo
{
}

#ifndef TODO_INFO
	#define TODO_INFO(...) (std::cout << std::format(__VA_ARGS__) << std::endl)
#endif

#ifndef TODO_ERROR
	#define TODO_ERROR(...) (std::cerr << std::format(__VA_ARGS__) << std::endl)
#endif

#ifndef TODO_ERR
	#define TODO_ERR(...) TODO_ERROR(__VA_ARGS__)
#endif

#ifndef TODO_ASSERT
	#include <cassert>
	#define TODO_ASSERT(condition,...) if(!(condition)) {TODO_ERROR(__VA_ARGS__); assert(condition);}
#endif