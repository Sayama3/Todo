//
// Created by Sayama on 30/08/2024.
//

#pragma once

#include "Concepts.hpp"

namespace Todo {
	template<LessThanComparable T>
	[[maybe_unused]] constexpr T Clamp(T value, T min_, T max_) noexcept {if(max_ < min_) return Clamp(value, max_, min_); return std::min(max_, std::max(min_, value));}


	// template<typename Clock, typename Rep, typename Period>
	// constexpr std::chrono::time_point<Clock, std::chrono::duration<Rep, Period>> ClockNow() {
	// 	return std::chrono::time_point<Clock, std::chrono::duration<Rep, Period>>(std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(Clock::now().time_since_epoch()));
	// }

	template<StdClock Clock, typename Duration>
	constexpr std::chrono::time_point<Clock, Duration> ClockNow() {
		return std::chrono::time_point<Clock, Duration>(std::chrono::duration_cast<Duration>(Clock::now().time_since_epoch()));
	}
}