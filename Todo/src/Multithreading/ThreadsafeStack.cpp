//
// Created by ianpo on 01/09/2024.
//

#include "Todo/Multithreading/ThreadsafeStack.hpp"

namespace Todo
{
	empty_stack::empty_stack() : std::logic_error("The stack is empty and shouldn't.")
	{
	}
}