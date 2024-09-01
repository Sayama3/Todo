//
// Created by ianpo on 01/09/2024.
//

#pragma once

#include "TMutex.hpp"

namespace Todo {
	using Mutex = TMutex<std::mutex>;
} // Todo
