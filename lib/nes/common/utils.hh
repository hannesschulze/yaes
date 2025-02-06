#pragma once

#include "nes/common/types.hh"

namespace nes
{
	template<typename T>
	auto launder(T* p) -> T*
	{
		return __builtin_launder(p);
	}

	template<typename T>
	auto copy(T const* src, T* dst, u32 const length)
	{
		for (auto i = u32{ 0 }; i < length; ++i) { dst[i] = src[i]; }
	}

	template<typename T>
	auto min(T const a, T const b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	auto max(T const a, T const b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	auto swap(T& a, T& b) -> void
	{
		auto const tmp = a;
		a = b;
		b = tmp;
	}
} // namespace nes