//
// Created by ianpo on 30/08/2024.
//

#pragma once

namespace Todo {
	template<typename T>
	[[maybe_unused]] constexpr T Clamp(T value, T min_, T max_) noexcept {if(max_ < min_) return Clamp(value, max_, min_); return std::min(max_, std::max(min_, value));}
}