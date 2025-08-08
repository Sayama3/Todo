//
// Created by ianpo on 07/08/2025.
//

#pragma once

#include "Concepts.hpp"
#include "Mutex.hpp"

namespace Todo {
	/// A map that can safely be accessed across multiple threads using locks.
	/// @tparam Key The Trivial Key Type
	/// @tparam Value The Movable Value
	template<Trivial Key, std::movable Value, typename Hash = std::hash<Key>>
	class ThreadsafeMap {

	};

}