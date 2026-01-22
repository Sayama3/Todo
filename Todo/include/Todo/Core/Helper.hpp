//    Copyright 2026 https://github.com/Sayama3
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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