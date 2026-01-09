//
// Created by Sayama on 01/09/2024.
//

#pragma once

#include "Concepts.hpp"
#include "SpinlockMutex.hpp"
#include "TMutex.hpp"

namespace Todo {
	using StdMutex = TMutex<std::mutex>;
} // Todo
