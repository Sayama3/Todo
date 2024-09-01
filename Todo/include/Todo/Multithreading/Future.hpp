//
// Created by ianpo on 31/08/2024.
//

#pragma once

// TODO: Once the std::future<T>::is_ready become available,
//  use that as the wait_for/wait_until can have quite a lot of
//  overhead depending on the compiler and the OS.
//  (Like from a few ns to more than a few minutes.)

namespace Todo
{
	template<typename T>
	std::future_status GetStatus(std::future<T>& future) { return future.wait_for(std::chrono::seconds(0)); }

	template<typename T>
	bool IsReady(std::future<T>& future) { auto status = GetStatus(future); return status == std::future_status::ready || status == std::future_status::deferred; }
} // Todo
