//
// Created by ianpo on 31/07/2025.
//

#pragma once

namespace Todo {

	template<typename T>
	concept CMutex = requires(T& mut) {
		{mut.lock()} -> void;
		{mut.unlock()} -> void;
		{mut.try_lock()} noexcept -> bool;
	};

}
