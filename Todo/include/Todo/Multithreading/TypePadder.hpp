//
// Created by ianpo on 31/07/2025.
//

#pragma once

// Theses types are some helper structures to help me align types for usage in atomics where it's better to have value aligned
// to 8 bytes rather than 4 because most architecture might have an hardware atomics for 16 bytes types but not 12.
// This way, I can just Wrap it in some "ModPadd8" structure and be done with it.

// In addition to the generic types I have some concept and

namespace Todo {

	template< typename T, std::size_t padding>
	concept alignable = requires()
	{
		alignof(T) <= padding;
	};

	template<std::size_t value>
	concept not_zero = requires()
	{
		value != 0;
	};

	template<typename T, std::size_t padding>
	static inline consteval std::size_t ModAlign() noexcept {
		return alignof(T) % padding == 0 ? alignof(T) : (alignof(T) / padding + 1) * padding;
	}

	template<typename T, std::size_t padding> requires alignable<T, padding>
	struct alignas(padding) Padder {
		T value;
	};

	template<typename T, std::size_t padding> requires not_zero<padding>
	struct alignas(ModAlign<T,padding>()) ModPadder {
		T value;
	};

	template<typename T, std::size_t padding>
	struct alignas(alignof(T) > padding ? alignof(T) : padding) MinPadder {
		T value;
	};

	template<typename T> requires alignable<T, 8>
	using Padder8 = Padder<T,8>;

	template<typename T> requires alignable<T, 16>
	using Padder16 = Padder<T,16>;

	template<typename T>
	using ModPadder4 = ModPadder<T,4>;

	template<typename T>
	using ModPadder8 = ModPadder<T,8>;

	template<typename T>
	using ModPadder16 = ModPadder<T,16>;

	template<typename T>
	using MinPadder8 = MinPadder<T,8>;

	template<typename T>
	using MinPadder16 = MinPadder<T,16>;
}