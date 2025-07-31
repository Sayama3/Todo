//
// Created by ianpo on 31/07/2025.
//

#pragma once

namespace Todo {

	template<typename T>
	concept CMutex = requires(T mut) {
		{mut.lock()};
		{mut.unlock()};
		{mut.try_lock()} noexcept -> std::convertible_to<bool>;
	};

}
